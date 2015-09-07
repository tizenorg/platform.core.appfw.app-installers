// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>
#include <boost/system/error_code.hpp>

#include <pkgmgr-info.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include <tzplatform_config.h>

#include <cassert>
#include <cstring>
#include <regex>
#include <string>
#include <vector>

#include "common/security_registration.h"
#include "common/utils/logging.h"

namespace bf = boost::filesystem;
namespace bpo = boost::program_options;
namespace bs = boost::system;
namespace ci = common_installer;

namespace {

const char* kEntries[] = {
  "/",
  "data/",
  "cache/"
};

const char kSkelAppDir[] = "/etc/skel/apps_rw";
const char kAppsRwLocation[] = "apps_rw";
const char kPackagePattern[] = "^[0-9a-zA-Z]{10}$";
const char kWgtType[] = "wgt";

bool ValidateTizenPackageId(const std::string& id) {
  std::regex package_regex(kPackagePattern);
  return std::regex_match(id, package_regex);
}

int PkgmgrListCallback(const pkgmgrinfo_pkginfo_h handle, void *user_data) {
  std::vector<std::string>* pkgids =
      reinterpret_cast<std::vector<std::string>*>(user_data);
  // filter non wgt packages
  char* type = nullptr;
  if (pkgmgrinfo_pkginfo_get_type(handle, &type) != PMINFO_R_OK)
    return -1;
  if (strcmp(type, kWgtType) != 0) {
    return 0;
  }

  char* pkgid = nullptr;
  if (pkgmgrinfo_pkginfo_get_pkgid(handle, &pkgid) != PMINFO_R_OK) {
    return -1;
  }
  pkgids->emplace_back(pkgid);
  return 0;
}

std::vector<std::string> GetAllGlobalApps() {
  std::vector<std::string> pkgids;
  if (pkgmgrinfo_pkginfo_get_usr_list(&PkgmgrListCallback,
      &pkgids, tzplatform_getuid(TZ_SYS_GLOBALAPP_USER)) != PMINFO_R_OK) {
    LOG(ERROR) << "Failed to query global application list";
    return {};
  }
  return pkgids;
}

class ScopedUser {
 public:
  ScopedUser(uid_t uid, gid_t gid) {
    uid_ = geteuid();
    gid_ = getegid();
    assert(seteuid(uid) == 0);
    assert(setegid(gid) == 0);
  }
  ~ScopedUser() {
    assert(seteuid(uid_) == 0);
    assert(setegid(gid_) == 0);
  }
 private:
  uid_t uid_;
  gid_t gid_;
};

bool CreateDirectories(const bf::path& app_dir, const std::string& pkgid,
                       uid_t uid, gid_t gid) {
  ScopedUser scoped_user(uid, gid);

  bf::path base_dir = app_dir / pkgid;
  if (bf::exists(base_dir)) {
    LOG(DEBUG) << "Directory for user already exist: " << base_dir;
    return true;
  }

  bs::error_code error;
  for (auto& subpart : kEntries) {
    bf::path subpath = base_dir / subpart;
    bf::create_directories(subpath, error);
    if (error) {
      LOG(ERROR) << "Failed to create directory: " << subpath;
      return false;
    }

    // set permissions
    bf::permissions(subpath,
                    bf::owner_all |
                    bf::group_read |
                    bf::group_exe |
                    bf::others_read |
                    bf::others_exe, error);
    if (error) {
      LOG(ERROR) << "Failed to set permissions for directory";
      return false;
    }

    // set user/group
    int ret = chown(subpath.c_str(), uid, gid);
    if (ret != 0) {
      LOG(ERROR) << "Failed to change owner of created directory";
      return false;
    }
  }

  // set smack
  if (!pkgid.empty()) {
    if (!common_installer::RegisterSecurityContext(pkgid, base_dir, nullptr)) {
      return false;
    }
  }

  return true;
}

bool CreatePerUserDirectories(const std::string& pkgid) {
  for (bf::directory_iterator it("/home"); it != bf::directory_iterator();
       ++it) {
    if (!bf::is_directory(it->path()))
      continue;
    std::string user = it->path().filename().string();
    struct passwd* pwd = getpwnam(user.c_str());  // NOLINT
    if (!pwd) {
      LOG(WARNING) << "Failed to get user for home directory: " << user;
      continue;
    }
    LOG(DEBUG) << "Creating directories for user: " << user;
    if (!CreateDirectories(it->path() / kAppsRwLocation, pkgid, pwd->pw_uid,
                           pwd->pw_gid))
      return false;
  }
  return true;
}

bool CreateSkelDirectories(const std::string& pkgid) {
  bf::path path = bf::path(kSkelAppDir) / pkgid;
  LOG(DEBUG) << "Creating directories in: " << path;
  bs::error_code error;
  bf::create_directories(path, error);
  if (error) {
    LOG(ERROR) << "Failed to delete directory: " << path;
    return false;
  }
  return true;
}

bool DeleteDirectories(const bf::path& app_dir, const std::string& pkgid,
                       uid_t uid, gid_t gid) {
  ScopedUser scoped_user(uid, gid);

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
  for (bf::directory_iterator it("/home"); it != bf::directory_iterator();
       ++it) {
    if (!bf::is_directory(it->path()))
      continue;
    std::string user = it->path().filename().string();
    struct passwd* pwd = getpwnam(user.c_str());  // NOLINT
    if (!pwd) {
      LOG(WARNING) << "Failed to get user for home directory: " << user;
      continue;
    }
    LOG(DEBUG) << "Deleting directories for user: " << user;
    if (!DeleteDirectories(it->path() / kAppsRwLocation, pkgid, pwd->pw_uid,
                           pwd->pw_gid))
      return false;
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

bool PerformDirectoryCreation(const std::string& pkgid) {
  if (!CreatePerUserDirectories(pkgid))
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

void ExclusiveOptions(const bpo::variables_map& vm,
                        const std::string& opt1, const std::string& opt2) {
    if ((vm.count(opt1) && vm.count(opt2)) ||
        (!vm.count(opt1) && !vm.count(opt2))) {
        throw std::logic_error(std::string("Exclusive options '") +
                               opt1 + "' and '" + opt2 + "'.");
    }
}

}  // namespace

int main(int argc, char** argv) {
  bpo::options_description options("Allowed options");
  options.add_options()
      ("create", "create per user diretories for global package")
      ("delete", "delete per user diretories for global package")
      ("allglobalpkgs", "install directories for all global applications")
      ("pkgid", bpo::value<std::string>(), "package ID");
  bpo::variables_map opt_map;
  bpo::store(bpo::parse_command_line(argc, argv, options), opt_map);
  ExclusiveOptions(opt_map, "create", "delete");
  ExclusiveOptions(opt_map, "pkgid", "allglobalpkgs");
  bpo::notify(opt_map);

  bool create_mode = opt_map.count("create") != 0;
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
  std::vector<std::string> pkgids;
  if (allglobalpkgs) {
    pkgids = GetAllGlobalApps();
  } else {
    pkgids.push_back(pkgid);
  }

  if (create_mode) {
    for (auto& package_id : pkgids) {
      LOG(DEBUG) << "Running for package id: " << package_id;
      if (!PerformDirectoryCreation(package_id))
        return -1;
    }
  } else if (delete_mode) {
    for (auto& package_id : pkgids) {
      LOG(DEBUG) << "Running for package id: " << package_id;
      if (!PerformDirectoryDeletion(package_id))
        return -1;
    }
  }
  return 0;
}
