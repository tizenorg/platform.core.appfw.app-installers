// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>
#include <boost/system/error_code.hpp>

#include <manifest_parser/utils/logging.h>
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

namespace bf = boost::filesystem;
namespace bpo = boost::program_options;
namespace bs = boost::system;
namespace ci = common_installer;

namespace {

struct pkg_info {
  pkg_info(std::string pkg_id_, std::string api_version_, std::string author_) :
    pkg_id(pkg_id_), api_version(api_version_), author_id(author_) {}

  std::string pkg_id;
  std::string api_version;
  std::string author_id;
};

using pkg_list = std::vector<pkg_info>;

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

bool ValidateTizenPackageId(const std::string& id) {
  std::regex package_regex(kPackagePattern);
  return std::regex_match(id, package_regex);
}

int PkgmgrListCallback(const pkgmgrinfo_pkginfo_h handle, void *user_data) {
  auto pkgs = reinterpret_cast<pkg_list*>(user_data);
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

pkg_list GetAllGlobalApps() {
  pkg_list pkgs;
  if (pkgmgrinfo_pkginfo_get_usr_list(&PkgmgrListCallback,
      &pkgs, tzplatform_getuid(TZ_SYS_GLOBALAPP_USER)) != PMINFO_R_OK) {
    LOG(ERROR) << "Failed to query global application list";
    return {};
  }
  return pkgs;
}

pkg_list GetPkg(const std::string& pkgid) {
  pkg_list pkgs;
  pkgmgrinfo_pkginfo_h handle;
  if (pkgmgrinfo_pkginfo_get_pkginfo(pkgid.c_str(), &handle) != PMINFO_R_OK) {
    return {};
  }
  if (PkgmgrListCallback(handle, &pkgs) != 0) {
    pkgmgrinfo_pkginfo_destroy_pkginfo(handle);
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
      if (!common_installer::RegisterSecurityContext(appid, pkgid,
          author_id, api_version, base_dir, uid, privileges,
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
                       const std::string& api_version,
                       uid_t uid, gid_t gid) {
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

    if (!SetPackageDirectoryOwnerAndPermissions(subpath, uid, gid))
      return false;

    // for content
    for (bf::recursive_directory_iterator iter(subpath);
         iter != bf::recursive_directory_iterator(); ++iter) {
      if (!SetPackageDirectoryOwnerAndPermissions(iter->path(), uid, gid))
        return false;
    }
  }

  if (!SetPackageDirectorySmackRules(base_dir, pkgid, author_id, api_version,
      uid))
    return false;

  return true;
}

bool CreatePerUserDirectories(const std::string& pkgid,
    const std::string& author_id, const std::string& api_version) {
  for (bf::directory_iterator iter("/home"); iter != bf::directory_iterator();
       ++iter) {
    if (!bf::is_directory(iter->path()))
      return false;
    const bf::path& home_path = iter->path();
    std::string user = home_path.filename().string();
    struct passwd* pwd = getpwnam(user.c_str());  // NOLINT
    if (!pwd) {
      LOG(WARNING) << "Failed to get user for home directory: " << user;
      continue;
    }

    struct group* gr = getgrgid(pwd->pw_gid);  // NOLINT
    if (strcmp(gr->gr_name, tzplatform_getenv(TZ_SYS_USER_GROUP)) != 0)
      continue;

    LOG(DEBUG) << "Creating directories for uid: " << pwd->pw_uid << ", gid: "
               << pwd->pw_gid << ", home: " << home_path;
    tzplatform_set_user(pwd->pw_uid);
    bf::path apps_rw(tzplatform_getenv(TZ_USER_APP));
    tzplatform_reset_user();
    if (!CreateDirectories(apps_rw, pkgid, author_id, api_version,
        pwd->pw_uid, pwd->pw_gid)) {
      return false;
    }
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

  // TODO(jungh.yeon) : this is hotfix.
  for (auto& entry : kEntries) {
    bf::path subpath = path / entry;
    bf::create_directories(subpath, error);
    std::string label = "User::Pkg::" + pkgid;
    if (error) {
      LOG(ERROR) << "Failed to create directory: " << subpath;
      return false;
    }

    int r =
        lsetxattr(subpath.c_str(), "security.SMACK64TRANSMUTE", "TRUE", 4, 0);
    if (r < 0) {
      LOG(ERROR) << "Failed to apply transmute";
      return false;
    }

    r = lsetxattr(subpath.c_str(), "security.SMACK64",
                  label.c_str(), label.length(), 0);
    if (r < 0) {
      LOG(ERROR) << "Failed to apply label";
      return false;
    }
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
  for (bf::directory_iterator iter("/home"); iter != bf::directory_iterator();
       ++iter) {
    if (!bf::is_directory(iter->path()))
      return false;
    const bf::path& home_path = iter->path();
    std::string user = home_path.filename().string();
    struct passwd* pwd = getpwnam(user.c_str());  // NOLINT
    if (!pwd) {
      LOG(WARNING) << "Failed to get user for home directory: " << user;
      continue;
    }

    struct group* gr = getgrgid(pwd->pw_gid);  // NOLINT
    if (strcmp(gr->gr_name, tzplatform_getenv(TZ_SYS_USER_GROUP)) != 0)
      continue;

    if (ci::IsPackageInstalled(pkgid, pwd->pw_uid)) continue;

    LOG(DEBUG) << "Deleting directories for uid: " << pwd->pw_uid << ", gid: "
               << pwd->pw_gid;
    tzplatform_set_user(pwd->pw_uid);
    bf::path apps_rw(tzplatform_getenv(TZ_USER_APP));
    tzplatform_reset_user();
    if (!DeleteDirectories(apps_rw, pkgid)) {
      return false;
    }
  }
  return true;
}

bool DeleteSkelDirectories(const std::string& pkgid) {
  bf::path path = bf::path(kSkelAppDir) / pkgid;
  LOG(DEBUG) << "Deleting directories in: " << path;
  bs::error_code error;
  bf::remove_all(path, error);
  if (error) {
    LOG(ERROR) << "Failed to delete directory: " << path;
    return false;
  }
  return true;
}

bool PerformDirectoryCreation(const std::string& pkgid,
    const std::string& author_id, const std::string& api_version) {
  if (!CreatePerUserDirectories(pkgid, author_id, api_version))
    return false;
  if (!CreateSkelDirectories(pkgid))
    return false;
  return true;
}

bool PerformDirectoryDeletion(const std::string& pkgid) {
  if (!DeletePerUserDirectories(pkgid))
    return false;
  if (!DeleteSkelDirectories(pkgid))
    return false;
  return true;
}

template<typename ... Arguments>
void ExclusiveOptions(const bpo::variables_map& vm, Arguments... args) {
  std::vector<std::string> exclusive_options {args...};
  sort(exclusive_options.begin(), exclusive_options.end());
  std::vector<std::string> given_options;
  std::transform(vm.begin(), vm.end(), std::back_inserter(given_options),
          [](const bpo::variables_map::value_type& pair) {return pair.first;});
  sort(given_options.begin(), given_options.end());

  std::vector<std::string> options_intersection;
  std::set_intersection(exclusive_options.begin(), exclusive_options.end(),
                        given_options.begin(), given_options.end(),
                        back_inserter(options_intersection));
  if (options_intersection.size() > 1) {
    std::string exception = std::string("Exclusive options :") +
        options_intersection[0] + " " + options_intersection[1];
    throw std::logic_error(exception);
  }
}


}  // namespace

int main(int argc, char** argv) {
  bpo::options_description options("Allowed options");
  options.add_options()
      ("create", "create per user diretories for global package")
      ("create-external-storage",
          "create per user diretories for global package on external storage")
      ("delete", "delete per user diretories for global package")
      ("allglobalpkgs", "install directories for all global applications")
      ("pkgid", bpo::value<std::string>(), "package ID");
  bpo::variables_map opt_map;
  try {
    bpo::store(bpo::parse_command_line(argc, argv, options), opt_map);
    ExclusiveOptions(opt_map, "create", "create-external-storage", "delete");
    ExclusiveOptions(opt_map, "pkgid", "allglobalpkgs");
    bpo::notify(opt_map);
  } catch(const std::exception& error) {
    LOG(ERROR) << error.what();
    return -1;
  }

  bool create_mode = opt_map.count("create") != 0;
  bool create_external_mode = opt_map.count("create-external-storage") != 0;
  bool delete_mode = opt_map.count("delete") != 0;
  bool allglobalpkgs = opt_map.count("allglobalpkgs") != 0;
  std::string pkgid;
  if (opt_map.count("pkgid")) {
    pkgid = opt_map["pkgid"].as<std::string>();
    if (!ValidateTizenPackageId(pkgid)) {
      LOG(ERROR) << "Pkgid is invalid";
      return -1;
    }
  }
  pkg_list pkgs;
  if (allglobalpkgs) {
    pkgs = GetAllGlobalApps();
  } else {
    pkgs = GetPkg(pkgid);
  }

  assert(setuid(0) == 0);

  if (create_mode) {
    for (auto& p : pkgs) {
      LOG(DEBUG) << "Running for package id: " << p.pkg_id;
      if (!PerformDirectoryCreation(p.pkg_id, p.api_version,
          p.author_id))

      if (!PerformDirectoryCreation(p.pkg_id, p.api_version,
                                    p.author_id))
        continue;
    }
  } else if (delete_mode) {
    for (auto& p : pkgs) {
      LOG(DEBUG) << "Running for package id: " << p.pkg_id;
      if (!PerformDirectoryDeletion(p.pkg_id))
        continue;
    }
  }
  return 0;
}
