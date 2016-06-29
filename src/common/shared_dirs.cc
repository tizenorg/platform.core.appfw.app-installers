// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/shared_dirs.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>
#include <boost/system/error_code.hpp>

#include <glib.h>
#include <gio/gio.h>
#include <manifest_parser/utils/logging.h>
#include <vcore/Certificate.h>
#include <pkgmgr-info.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <tzplatform_config.h>
#include <sys/xattr.h>
#include <gum/gum-user.h>
#include <gum/gum-user-service.h>
#include <gum/common/gum-user-types.h>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <exception>
#include <iterator>
#include <regex>
#include <string>
#include <utility>
#include <vector>
#include <tuple>

#include "common/paths.h"
#include "common/security_registration.h"
#include "common/pkgmgr_query.h"
#include "common/utils/base64.h"
#include "common/utils/file_util.h"
#include "common/utils/glist_range.h"

namespace bf = boost::filesystem;
namespace bpo = boost::program_options;
namespace bs = boost::system;
namespace ci = common_installer;

namespace {

typedef std::vector<std::tuple<uid_t, gid_t, bf::path>> user_list;
const std::vector<const char*> kEntries = {
  {"/"},
  {"cache/"},
  {"data/"},
  {"shared/"},
  {"shared/cache/"},
};

const char kTrustedDir[] = "shared/trusted";
const char kSkelAppDir[] = "/etc/skel/apps_rw";
const char kPackagePattern[] = R"(^[0-9a-zA-Z_-]+(\.?[0-9a-zA-Z_-]+)*$)";
const int32_t kPWBufSize = sysconf(_SC_GETPW_R_SIZE_MAX);
const int32_t kGRBufSize = sysconf(_SC_GETGR_R_SIZE_MAX);

bool ValidateTizenPackageId(const std::string& id) {
  std::regex package_regex(kPackagePattern);
  return std::regex_match(id, package_regex);
}

int PkgmgrListCallback(const pkgmgrinfo_pkginfo_h handle, void *user_data) {
  auto pkgs = reinterpret_cast<ci::PkgList*>(user_data);
  char* pkgid = nullptr;
  if (pkgmgrinfo_pkginfo_get_pkgid(handle, &pkgid) != PMINFO_R_OK) {
    return -1;
  }
  char* api_version;
  if (pkgmgrinfo_pkginfo_get_api_version(handle, &api_version) != PMINFO_R_OK) {
    return -1;
  }
  pkgmgrinfo_certinfo_h cert_handle;
  if (pkgmgrinfo_pkginfo_create_certinfo(&cert_handle) != PMINFO_R_OK) {
    return -1;
  }
  if (pkgmgrinfo_pkginfo_load_certinfo(pkgid, cert_handle, 0) != PMINFO_R_OK) {
    pkgmgrinfo_pkginfo_destroy_certinfo(cert_handle);
    return -1;
  }
  const char* author_cert;
  if (pkgmgrinfo_pkginfo_get_cert_value(cert_handle, PMINFO_AUTHOR_SIGNER_CERT,
      &author_cert) != PMINFO_R_OK) {
    pkgmgrinfo_pkginfo_destroy_certinfo(cert_handle);
    return -1;
  }
  if (author_cert) {
    ValidationCore::Certificate cert(author_cert,
        ValidationCore::Certificate::FORM_BASE64);
    unsigned char* public_key;
    size_t len;
    cert.getPublicKeyDER(&public_key, &len);
    std::string author_id =
        ci::EncodeBase64(reinterpret_cast<const char*>(public_key));
    pkgs->emplace_back(pkgid, api_version, author_id);
  } else {
    pkgs->emplace_back(pkgid, std::string(), std::string());
  }

  pkgmgrinfo_pkginfo_destroy_certinfo(cert_handle);

  return 0;
}

ci::PkgList GetAllGlobalAppsInformation() {
  ci::PkgList pkgs;
  if (pkgmgrinfo_pkginfo_get_usr_list(&PkgmgrListCallback,
      &pkgs, tzplatform_getuid(TZ_SYS_GLOBALAPP_USER)) != PMINFO_R_OK) {
    LOG(ERROR) << "Failed to query global application list";
    return {};
  }
  return pkgs;
}

ci::PkgList GetPkgInformation(uid_t uid, const std::string& pkgid) {
  if (!ValidateTizenPackageId(pkgid)) {
    LOG(DEBUG) << "Package id validation failed. pkgid = " << pkgid;
    return ci::PkgList();
  }

  ci::PkgList pkgs;
  pkgmgrinfo_pkginfo_h handle;
  if (pkgmgrinfo_pkginfo_get_usr_pkginfo(pkgid.c_str(), uid, &handle) !=
      PMINFO_R_OK) {
    LOG(DEBUG) << "pkgmgrinfo_pkginfo_get_pkginfo failed, for pkgid=" << pkgid;
    return {};
  }
  if (PkgmgrListCallback(handle, &pkgs) != 0) {
    pkgmgrinfo_pkginfo_destroy_pkginfo(handle);
    LOG(DEBUG) << "PkgmgrListCallback failed";
    return {};
  }
  pkgmgrinfo_pkginfo_destroy_pkginfo(handle);
  return pkgs;
}

bool SetPackageDirectoryOwnerAndPermissions(const bf::path& subpath, uid_t uid,
                                            gid_t gid) {
  bs::error_code error;
  bf::perms perms = bf::owner_read |
                    bf::owner_write |
                    bf::group_read;
  if (bf::is_directory(subpath)) {
    perms |= bf::owner_exe | bf::group_exe | bf::others_exe;
  }
  bf::permissions(subpath, perms, error);
  if (error) {
    LOG(ERROR) << "Failed to set permissions for: " << subpath;
    return false;
  }
  int fd = open(subpath.c_str(), O_RDONLY);
  if (fd < 0) {
    LOG(ERROR) << "Can't open directory : " << subpath;
    return false;
  }
  int ret = fchown(fd, uid, gid);
  close(fd);
  if (ret != 0) {
    LOG(ERROR) << "Failed to change owner of: " << subpath;
    return false;
  }
  return true;
}

bool CreateDirectories(const bf::path& app_dir, const std::string& pkgid,
                       bool trusted,
                       uid_t uid, gid_t gid, const bool set_permissions) {
  bf::path base_dir = app_dir / pkgid;
  if (bf::exists(base_dir)) {
    LOG(DEBUG) << "Directory for user already exist: " << base_dir;
    return true;
  }

  bs::error_code error;
  std::vector<const char*> dirs(kEntries);
  if (trusted)
    dirs.push_back(kTrustedDir);
  for (auto& entry : dirs) {
    bf::path subpath = base_dir / entry;
    bf::create_directories(subpath, error);
    if (error) {
      LOG(ERROR) << "Failed to create directory: " << subpath;
      return false;
    }

    if (set_permissions) {
      if (!SetPackageDirectoryOwnerAndPermissions(subpath, uid, gid))
        return false;

      // for content
      for (bf::recursive_directory_iterator iter(subpath);
           iter != bf::recursive_directory_iterator(); ++iter) {
        if (!SetPackageDirectoryOwnerAndPermissions(iter->path(), uid, gid))
          return false;
      }
    }
  }

  return true;
}

bf::path GetDirectoryPathForStorage(uid_t user, std::string apps_prefix) {
  struct passwd pwd;
  struct passwd *pwd_result;
  char buf[kPWBufSize];
  int ret = getpwuid_r(user, &pwd, buf, sizeof(buf), &pwd_result);
  if (ret != 0 || pwd_result == nullptr)
    return {};

  bf::path apps_rw;
  apps_rw = bf::path(apps_prefix.c_str()) / pwd.pw_name / "apps_rw";

  return apps_rw;
}

bool CreateUserDirectories(uid_t user, const std::string& pkgid,
    bool trusted,
    const std::string& apps_prefix, const bool set_permissions) {
  struct passwd pwd;
  struct passwd *pwd_result;
  char buf_pw[kPWBufSize];
  int ret = getpwuid_r(user, &pwd, buf_pw, sizeof(buf_pw), &pwd_result);
  if (ret != 0 || pwd_result == nullptr) {
    LOG(WARNING) << "Failed to get user for home directory: " << user;
    return false;
  }

  struct group gr;
  struct group *gr_result;
  char buf_gr[kGRBufSize];
  ret = getgrgid_r(pwd.pw_gid, &gr, buf_gr, sizeof(buf_gr), &gr_result);
  if (ret != 0
      || strcmp(gr.gr_name, tzplatform_getenv(TZ_SYS_USER_GROUP)) != 0)
    return false;

  LOG(DEBUG) << "Creating directories for uid: " << pwd.pw_uid << ", gid: "
             << pwd.pw_gid;

  bf::path apps_rw = GetDirectoryPathForStorage(user, apps_prefix);
  if (apps_rw.empty()) {
    LOG(DEBUG) << "Directory not exists: " << apps_rw;
    return false;
  }

  if (!CreateDirectories(apps_rw, pkgid, trusted,
      pwd.pw_uid, pwd.pw_gid, set_permissions)) {
    return false;
  }
  return true;
}

bool DeleteDirectories(const bf::path& app_dir, const std::string& pkgid) {
  bf::path base_dir = app_dir / pkgid;
  bs::error_code error;
  bf::remove_all(base_dir, error);
  if (error) {
    LOG(ERROR) << "Failed to delete directory: " << base_dir;
    return false;
  }
  return true;
}

user_list GetUserList() {
  GumUserService* service =
      gum_user_service_create_sync((getuid() == 0) ? TRUE : FALSE);
  gchar** user_type_strv = gum_user_type_to_strv(
      GUM_USERTYPE_ADMIN | GUM_USERTYPE_GUEST | GUM_USERTYPE_NORMAL);
  GumUserList* gum_user_list =
      gum_user_service_get_user_list_sync(service, user_type_strv);
  user_list list;
  for (GumUser* guser : GListRange<GumUser*>(gum_user_list)) {
    uid_t uid;
    g_object_get(G_OBJECT(guser), "uid", &uid, nullptr);
    gid_t gid;
    g_object_get(G_OBJECT(guser), "gid", &gid, nullptr);
    gchar* homedir = nullptr;
    g_object_get(G_OBJECT(guser), "homedir", &homedir, nullptr);
    if (homedir == nullptr) {
      LOG(WARNING) << "No homedir for uid: " << uid;
      continue;
    }
    list.emplace_back(uid, gid, bf::path(homedir));
  }
  g_strfreev(user_type_strv);
  gum_user_service_list_free(gum_user_list);
  return list;
}

}  // namespace

namespace common_installer {

std::string GetDirectoryPathForInternalStorage() {
  const char* internal_storage_prefix = tzplatform_getenv(TZ_SYS_HOME);
  if (internal_storage_prefix)
    return std::string(internal_storage_prefix);
  return tzplatform_getenv(TZ_SYS_HOME);
}

std::string GetDirectoryPathForExternalStorage() {
  return GetExternalCardPath().string();
}

bool PerformInternalDirectoryCreationForUser(uid_t user,
                                             const std::string& pkgid,
                                             bool trusted) {
  const char* internal_storage_prefix = tzplatform_getenv(TZ_SYS_HOME);
  const bool set_permissions = true;
  if (!CreateUserDirectories(user, pkgid, trusted,
                             internal_storage_prefix, set_permissions))
    return false;
  return true;
}

bool PerformExternalDirectoryCreationForUser(uid_t user,
                                             const std::string& pkgid) {
  bf::path storage_path = GetExternalCardPath();

  // TODO(t.iwanek): trusted in this context means that we have signature
  // this argument is not longer needed as all package must be signed
  // so that trusted directory may be labeled correctly by security-manager in
  // all cases. This parameter and its propagation should be removed.
  bool trusted = true;

  const bool set_permissions = false;
  if (!bf::exists(storage_path)) {
    LOG(WARNING) << "External storage (SD Card) is not mounted.";
    return false;
  }

  bf::path storage_apps_path = storage_path / "apps";
  if (!bf::exists(storage_apps_path)) {
    bs::error_code error;
    bf::create_directories(storage_apps_path, error);
    if (error) {
      LOG(ERROR) << "Failed to create directory: "
          << storage_apps_path.c_str();
      return false;
    }
  }

  if (CreateUserDirectories(user, pkgid, trusted,
                            storage_apps_path.c_str(), set_permissions)) {
  }
  return true;
}

bool PerformExternalDirectoryDeletionForUser(uid_t user,
                                             const std::string& pkgid) {
  bf::path storage_path = GetExternalCardPath();
  if (!bf::exists(storage_path)) {
    LOG(WARNING) << "External storage (SD Card) is not mounted.";
    return false;
  }

  bf::path storage_apps_path = bf::path(storage_path) / "apps";
  return DeleteDirectories(
      GetDirectoryPathForStorage(user, storage_apps_path.string()), pkgid);
}

bool PerformInternalDirectoryCreationForAllUsers(const std::string& pkgid,
                                                 bool trusted) {
  user_list list = GetUserList();
  for (auto l : list) {
    if (!PerformInternalDirectoryCreationForUser(std::get<0>(l),
                                                 pkgid,
                                                 trusted))
      LOG(ERROR) << "Could not create internal storage directories for user: "
                 << std::get<0>(l);
  }
  return true;
}

bool PerformExternalDirectoryCreationForAllUsers(const std::string& pkgid) {
  user_list list = GetUserList();
  for (auto l : list) {
    if (!PerformExternalDirectoryCreationForUser(std::get<0>(l),
                                                 pkgid))
      LOG(WARNING) << "Could not create external storage directories for user: "
                   << std::get<0>(l);
  }
  return true;
}

bool PerformExternalDirectoryDeletionForAllUsers(const std::string& pkgid) {
  user_list list = GetUserList();
  for (auto l : list) {
    uid_t uid = std::get<0>(l);
    LOG(DEBUG) << "Deleting directories for user: " << uid;
    if (QueryIsPackageInstalled(pkgid, uid)) {
      LOG(DEBUG) << "Package: " << pkgid << " for uid: " << uid
                 << " still exists. Skipping";
      continue;
    }

    if (!PerformExternalDirectoryDeletionForUser(uid, pkgid))
      LOG(WARNING) << "Could not delete external storage directories for user: "
                   << uid;
  }
  return true;
}

bool CreateSkelDirectories(const std::string& pkgid) {
  bf::path path = bf::path(kSkelAppDir) / pkgid;
  LOG(DEBUG) << "Creating directories in: " << path;
  bs::error_code error;
  bf::create_directories(path, error);

  if (error) {
    LOG(ERROR) << "Failed to create directory: " << path;
    return false;
  }

  for (auto& entry : kEntries) {
    bf::path subpath = path / entry;
    bf::create_directories(subpath, error);
    if (error && !bf::exists(subpath)) {
      LOG(ERROR) << "Failed to create directory: " << subpath;
      return false;
    }
  }

  std::string error_message;
  if (!RegisterSecurityContextForPath(pkgid, path,
      tzplatform_getuid(TZ_SYS_GLOBALAPP_USER), true, &error_message)) {
    LOG(ERROR) << "Failed to register security context for path: " << path
               << ", error_message: " << error_message;
    return false;
  }

  return true;
}


bool DeleteSkelDirectories(const std::string& pkgid) {
  return DeleteDirectories(bf::path(kSkelAppDir), pkgid);
}


bool DeleteUserDirectories(const std::string& pkgid) {
  user_list list = GetUserList();
  for (auto l : list) {
    if (ci::QueryIsPackageInstalled(pkgid, std::get<0>(l))) {
      LOG(INFO) << pkgid << " is installed for user " << std::get<0>(l);
      continue;
    }

    LOG(DEBUG) << "Deleting directories of " << pkgid
               << ", for uid: " << std::get<0>(l);
    bf::path apps_rw(std::get<2>(l) / "apps_rw");
    if (!DeleteDirectories(apps_rw, pkgid)) {
      return false;
    }
  }
  return true;
}

bool DeleteUserExternalDirectories(const std::string& pkgid) {
  user_list list = GetUserList();
  for (auto l : list) {
    if (ci::QueryIsPackageInstalled(pkgid, std::get<0>(l))) {
      LOG(INFO) << pkgid << " is installed for user " << std::get<0>(l);
      continue;
    }

    LOG(DEBUG) << "Deleting external directories of " << pkgid
               << ", for uid: " << std::get<0>(l);
    bf::path apps_rw(std::get<2>(l) / "apps_rw");
    if (!DeleteDirectories(apps_rw, pkgid)) {
      return false;
    }
  }
  return true;
}


bool CopyUserDirectories(const std::string& pkgid) {
  user_list list = GetUserList();
  for (auto l : list) {
    LOG(DEBUG) << "Copying directories for uid: " << std::get<0>(l);
    bf::path apps_rw(std::get<2>(l) / "apps_rw");
    bf::path src = bf::path(kSkelAppDir) / pkgid;
    bf::path dst = apps_rw / pkgid;
    if (!ci::CopyDir(src, dst))
      continue;
    if (!SetPackageDirectoryOwnerAndPermissions(dst, std::get<0>(l),
        std::get<1>(l)))
      return false;
    for (bf::recursive_directory_iterator iter(dst);
        iter != bf::recursive_directory_iterator(); ++iter) {
      if (!SetPackageDirectoryOwnerAndPermissions(iter->path(),
          std::get<0>(l), std::get<1>(l)))
        return false;
    }
    std::string error_message;
    if (!RegisterSecurityContextForPath(pkgid, dst, std::get<0>(l), false,
        &error_message)) {
      LOG(ERROR) << "Failed to register security context for path: " << dst
                 << ", error_message: " << error_message;
      return false;
    }
  }
  return true;
}

ci::PkgList CreatePkgInformationList(uid_t uid,
                                     const std::vector<std::string>& pkgs) {
  return pkgs.empty() ?
      GetAllGlobalAppsInformation() : GetPkgInformation(uid, *pkgs.begin());
}

}  // namespace common_installer
