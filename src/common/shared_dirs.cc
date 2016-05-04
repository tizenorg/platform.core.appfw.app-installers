// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/shared_dirs.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>
#include <boost/system/error_code.hpp>

#include <vcore/Certificate.h>
#include <pkgmgr-info.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <unistd.h>
#include <tzplatform_config.h>
#include <sys/xattr.h>


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

#include "common/security_registration.h"
#include "common/pkgmgr_registration.h"
#include "common/utils/base64.h"
#include "common/utils/file_util.h"
#include "common/utils/glist_range.h"
#include "common/utils/logger.h"

namespace bf = boost::filesystem;
namespace bpo = boost::program_options;
namespace bs = boost::system;
namespace ci = common_installer;

namespace {

const std::vector<const char*> kEntries = {
  {"/"},
  {"cache/"},
  {"data/"},
  {"shared/"},
  {"shared/cache/"},
};

const char kTrustedDir[] = "shared/trusted";
const char kPackagePattern[] = R"(^[0-9a-zA-Z_-]+(\.?[0-9a-zA-Z_-]+)*$)";
const char kExternalStorageDirPrefix[] = "sdcard";

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

bool SetPackageDirectorySmackRules(const bf::path& base_dir,
                                   const std::string& pkgid,
                                   const std::string& author_id,
                                   const std::string& api_version,
                                   uid_t uid) {
  if (!pkgid.empty()) {
    std::vector<std::string> privileges;
    std::vector<std::string> appids;
    if (!common_installer::QueryPrivilegesForPkgId(pkgid,
        tzplatform_getuid(TZ_SYS_GLOBALAPP_USER), &privileges)) {
      LOG(ERROR) << "Failed to get privileges for package id";
      return false;
    }
    if (!common_installer::QueryAppidsForPkgId(pkgid, &appids,
        tzplatform_getuid(TZ_SYS_GLOBALAPP_USER))) {
      LOG(ERROR) << "Failed to get application ids for package id";
      return false;
    }
    std::string error_message;
    for (const auto& appid : appids) {
      bf::path pkg_dir = base_dir / pkgid;
      if (!common_installer::RegisterSecurityContext(appid, pkgid,
          author_id, api_version, ci::SecurityAppInstallType::Local,
          pkg_dir, uid, privileges,
          &error_message)) {
        LOG(ERROR) << "Failed to register security context";
        if (!error_message.empty()) {
          LOG(ERROR) << "error_message: " << error_message;
        }
        return false;
      }
    }
  }
  return true;
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
  int ret = chown(subpath.c_str(), uid, gid);
  if (ret != 0) {
    LOG(ERROR) << "Failed to change owner of: " << subpath;
    return false;
  }
  return true;
}

bool CreateDirectories(const bf::path& app_dir, const std::string& pkgid,
                       const std::string& author_id,
                       uid_t uid, gid_t gid, const bool set_permissions) {
  bf::path base_dir = app_dir / pkgid;
  if (bf::exists(base_dir)) {
    LOG(DEBUG) << "Directory for user already exist: " << base_dir;
    return true;
  }

  bs::error_code error;
  std::vector<const char*> dirs(kEntries);
  if (!author_id.empty())
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
  struct passwd pwd, *pwd_result;
  char buf[1024] = {0, };

  int ret = getpwuid_r(user, &pwd, buf, sizeof(buf), &pwd_result);
  if (ret != 0 || pwd_result == NULL)
    return {};

  bf::path apps_rw;
  apps_rw = bf::path(apps_prefix.c_str()) / pwd.pw_name / "apps_rw";

  return apps_rw;
}

bool CreateUserDirectories(uid_t user, const std::string& pkgid,
    const std::string& author_id,
    const std::string& apps_prefix, const bool set_permissions) {
  char buf[1024] = {0, };

  struct passwd pwd, *pwd_result;
  int ret = getpwuid_r(user, &pwd, buf, sizeof(buf), &pwd_result);
  if (ret != 0 || pwd_result == NULL) {
    LOG(WARNING) << "Failed to get user for home directory: " << user;
    return false;
  }

  struct group gr, *gr_result;
  ret = getgrgid_r(pwd.pw_gid, &gr, buf, sizeof(buf), &gr_result);
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

  if (!CreateDirectories(apps_rw, pkgid, author_id,
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

bool DeletePerUserDirectories(const std::string& pkgid) {
  char buf[1024] = {0, };

  for (bf::directory_iterator iter(tzplatform_getenv(TZ_SYS_HOME));
      iter != bf::directory_iterator();
       ++iter) {
    if (!bf::is_directory(iter->path()))
      return false;
    const bf::path& home_path = iter->path();
    std::string user = home_path.filename().string();
    struct passwd pwd, *pwd_result;
    int ret = getpwnam_r(user.c_str(), &pwd, buf, sizeof(buf), &pwd_result);
    if (ret != 0 || pwd_result == NULL) {
      LOG(WARNING) << "Failed to get user for home directory: " << user;
      continue;
    }

    struct group gr, *gr_result;
    ret = getgrgid_r(pwd.pw_gid, &gr, buf, sizeof(buf), &gr_result);
    if (ret != 0 || gr_result == NULL ||
        strcmp(gr.gr_name, tzplatform_getenv(TZ_SYS_USER_GROUP)) != 0)
      continue;

    if (ci::IsPackageInstalled(pkgid, pwd.pw_uid)) continue;

    std::string error_message;
    if (!ci::UnregisterSecurityContextForPkgId(pkgid, pwd.pw_uid,
        &error_message)) {
      LOG(WARNING) << "Failure on unregistering security context for pkg: "
                   << pkgid << ", uid: " << pwd.pw_uid;
    }

    LOG(DEBUG) << "Deleting directories for uid: " << pwd.pw_uid << ", gid: "
               << pwd.pw_gid;
    tzplatform_set_user(pwd.pw_uid);
    bf::path apps_rw(tzplatform_getenv(TZ_USER_APP));
    tzplatform_reset_user();
    if (!DeleteDirectories(apps_rw, pkgid)) {
      return false;
    }
  }
  return true;
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
  const char* storage_path = tzplatform_mkpath(TZ_SYS_MEDIA,
                                               kExternalStorageDirPrefix);
  return std::string(storage_path);
}

bool PerformInternalDirectoryCreationForUser(uid_t user,
                                             const std::string& pkgid,
                                             const std::string& author_id) {
  const char* internal_storage_prefix = tzplatform_getenv(TZ_SYS_HOME);
  const bool set_permissions = true;
  if (!CreateUserDirectories(user, pkgid, author_id,
                             internal_storage_prefix, set_permissions))
    return false;
  return true;
}

bool PerformExternalDirectoryCreationForUser(uid_t user,
                                             const std::string& pkgid,
                                             const std::string& author_id) {
  const char* storage_path = tzplatform_mkpath(TZ_SYS_MEDIA,
                                               kExternalStorageDirPrefix);
  const bool set_permissions = false;
  if (!bf::exists(storage_path)) {
    LOG(WARNING) << "External storage (SD Card) is not mounted.";
    return false;
  }

  if (CreateUserDirectories(user, pkgid, author_id,
                            storage_path, set_permissions)) {
  }
  return true;
}

bool PerformInternalDirectoryCreationForAllUsers(const std::string& pkgid,
                                                 const std::string& author_id) {
  char buf[1024] = {0, };

  for (bf::directory_iterator iter(tzplatform_getenv(TZ_SYS_HOME));
      iter != bf::directory_iterator();
         ++iter) {
    if (!bf::is_directory(iter->path()))
        continue;
    const bf::path& home_path = iter->path();
    std::string user = home_path.filename().string();

    struct passwd pwd, *pwd_result;
    int ret = getpwnam_r(user.c_str(), &pwd, buf, sizeof(buf), &pwd_result);
    if (ret != 0 || pwd_result == NULL)
      continue;

    struct group gr, *gr_result;
    ret = getgrgid_r(pwd.pw_gid, &gr, buf, sizeof(buf), &gr_result);
    if (ret != 0 ||
        strcmp(gr.gr_name, tzplatform_getenv(TZ_SYS_USER_GROUP)) != 0)
      continue;

    if (!PerformInternalDirectoryCreationForUser(pwd.pw_uid,
                                                 pkgid,
                                                 author_id))
      LOG(ERROR) << "Could not create internal storage directories for user: "
        << user.c_str();
  }
  return true;
}

bool PerformExternalDirectoryCreationForAllUsers(const std::string& pkgid,
                                                 const std::string& author_id) {
  char buf[1024] = {0, };

  for (bf::directory_iterator iter(tzplatform_getenv(TZ_SYS_HOME));
      iter != bf::directory_iterator();
         ++iter) {
    try {
      if (!bf::is_directory(iter->path())) continue;
    }
    catch (const bf::filesystem_error&) {
      continue;
    }

    const bf::path& home_path = iter->path();
    std::string user = home_path.filename().string();

    struct passwd pwd, *pwd_result;
    int ret = getpwnam_r(user.c_str(), &pwd, buf, sizeof(buf), &pwd_result);
    if (ret != 0 || pwd_result == NULL)
      continue;

    struct group gr, *gr_result;
    ret = getgrgid_r(pwd.pw_gid, &gr, buf, sizeof(buf), &gr_result);
    if (ret != 0 ||
        strcmp(gr.gr_name, tzplatform_getenv(TZ_SYS_USER_GROUP)) != 0)
      continue;

    if (!PerformExternalDirectoryCreationForUser(pwd.pw_uid,
                                                 pkgid,
                                                 author_id))
      LOG(WARNING) << "Could not create external storage directories for user: "
        << user.c_str();
  }
  return true;
}

bool SetPackageDirectorySmackRulesForUser(uid_t uid,
                                          const std::string& pkg_id,
                                          const std::string& author_id,
                                          const std::string& api_version) {
  char buf[1024];
  struct passwd pwd;
  struct passwd *pwd_result;
  int ret = getpwuid_r(uid, &pwd, buf, sizeof(buf), &pwd_result);
  if (ret != 0 || pwd_result == NULL) {
    LOG(WARNING) << "Failed to get user for home directory: " << uid;
    return false;
  }

  std::string base_path = std::string(pwd.pw_dir) + "/apps_rw";

  return SetPackageDirectorySmackRules(base_path,
                                       pkg_id,
                                       author_id,
                                       api_version,
                                       uid);
}


bool SetPackageDirectorySmackRulesForAllUsers(const std::string& pkg_id,
                                              const std::string& author_id,
                                              const std::string& api_version) {
  char buf[1024] = {0, };

  for (bf::directory_iterator iter(tzplatform_getenv(TZ_SYS_HOME));
      iter != bf::directory_iterator();
         ++iter) {
    if (!bf::is_directory(iter->path()))
        continue;
    const bf::path& home_path = iter->path();
    std::string user = home_path.filename().string();

    struct passwd pwd, *pwd_result;
    int ret = getpwnam_r(user.c_str(), &pwd, buf, sizeof(buf), &pwd_result);
    if (ret != 0 || pwd_result == NULL) {
      LOG(WARNING) << "Failed to get user for home directory: " << user;
      return false;
    }

    if (!SetPackageDirectorySmackRulesForUser(pwd.pw_uid,
                                              pkg_id,
                                              author_id,
                                              api_version)) {
      LOG(WARNING) << "Failed to set directory smack rules for user: " << user;
      continue;
    }
  }
  return true;
}


bool PerformDirectoryDeletionForAllUsers(const std::string& pkgid) {
  if (!DeletePerUserDirectories(pkgid))
    return false;
  return true;
}

ci::PkgList CreatePkgInformationList(uid_t uid,
                                     const std::vector<std::string>& pkgs) {
  return pkgs.empty() ?
      GetAllGlobalAppsInformation() : GetPkgInformation(uid, *pkgs.begin());
}

}  // namespace common_installer
