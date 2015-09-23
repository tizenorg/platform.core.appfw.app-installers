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
#include <exception>
#include <regex>
#include <string>
#include <utility>
#include <vector>

#include "common/security_registration.h"
#include "common/utils/file_util.h"
#include "common/utils/logging.h"

namespace bf = boost::filesystem;
namespace bpo = boost::program_options;
namespace bs = boost::system;
namespace ci = common_installer;

namespace {

enum class Action {
  CREATE,
  COPY_OR_CREATE
};

const std::vector<std::pair<const char*, Action>> kEntries = {
  {"/", Action::CREATE},
  {"cache/", Action::CREATE},
  {"data/", Action::COPY_OR_CREATE}  // compatibility -> copy data/ dir for tpk
};

const char kPackagePattern[] = "^[0-9a-zA-Z]{10}$";
const char kWgtType[] = "wgt";
const char kTpkType[] = "tpk";

bool ValidateTizenPackageId(const std::string& id) {
  std::regex package_regex(kPackagePattern);
  return std::regex_match(id, package_regex);
}

int PkgmgrListCallback(const pkgmgrinfo_pkginfo_h handle, void *user_data) {
  std::vector<std::string>* pkgids =
      reinterpret_cast<std::vector<std::string>*>(user_data);
  // filter non wgt/tpk packages
  char* type = nullptr;
  if (pkgmgrinfo_pkginfo_get_type(handle, &type) != PMINFO_R_OK)
    return -1;
  if (strcmp(type, kWgtType) != 0 && strcmp(type, kTpkType) != 0) {
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

bool CreateDirectories(const bf::path& app_dir, const std::string& pkgid) {
  bf::path base_dir = app_dir / pkgid;
  if (bf::exists(base_dir)) {
    LOG(DEBUG) << "Directory for user already exist: " << base_dir;
    return true;
  }

  bs::error_code error;
  for (auto& pair : kEntries) {
    bf::path subpath = base_dir / pair.first;
    switch (pair.second) {
    case Action::COPY_OR_CREATE: {
      bf::path global_directory =
          bf::path(tzplatform_getenv(TZ_SYS_RW_APP)) / pkgid / pair.first;
      if (bf::exists(global_directory)) {
        if (!ci::CopyDir(global_directory, subpath)) {
          LOG(ERROR) << "Failed to copy directory: " << global_directory;
          return false;
        }
        break;
      }
    }
    case Action::CREATE: {
      bf::create_directories(subpath, error);
      if (error) {
        LOG(ERROR) << "Failed to create directory: " << subpath;
        return false;
      }
      break;
    }
    default:
      assert(false);
    }

    bf::permissions(subpath,
                    bf::owner_read | bf::owner_write | bf::owner_exe |
                    bf::group_read | bf::group_exe, error);
    if (error) {
      LOG(ERROR) << "Failed to set permissions for: " << subpath;
      return false;
    }

    // for content
    for (bf::recursive_directory_iterator iter(subpath);
         iter != bf::recursive_directory_iterator(); ++iter) {
      bf::perms perms = bf::owner_read |
                        bf::owner_write |
                        bf::group_read;
      if (bf::is_directory(iter->path())) {
        perms |= bf::owner_exe | bf::group_exe | bf::others_exe;
      }
      bf::permissions(iter->path(), perms, error);
      if (error) {
        LOG(ERROR) << "Failed to set permissions for: " << iter->path();
        return false;
      }
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
  bf::path path(tzplatform_getenv(TZ_USER_APP));
  if (!CreateDirectories(path, pkgid))
    return false;
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
  bf::path path(tzplatform_getenv(TZ_USER_APP));
  if (!DeleteDirectories(path, pkgid))
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
  uid_t uid = getuid();
  if (uid == 0 || uid == tzplatform_getuid(TZ_SYS_GLOBALAPP_USER)) {
    LOG(ERROR) << "Binary shouldn't be run for root and tizenglobalapp";
    return -1;
  }

  bpo::options_description options("Allowed options");
  options.add_options()
      ("create", "create per user diretories for global package")
      ("delete", "delete per user diretories for global package")
      ("allglobalpkgs", "install directories for all global applications")
      ("pkgid", bpo::value<std::string>(), "package ID");
  bpo::variables_map opt_map;
  try {
    bpo::store(bpo::parse_command_line(argc, argv, options), opt_map);
    ExclusiveOptions(opt_map, "create", "delete");
    ExclusiveOptions(opt_map, "pkgid", "allglobalpkgs");
    bpo::notify(opt_map);
  } catch(const std::exception& error) {
    LOG(ERROR) << error.what();
    return -1;
  }

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
      if (!CreatePerUserDirectories(package_id))
        return -1;
    }
  } else if (delete_mode) {
    for (auto& package_id : pkgids) {
      LOG(DEBUG) << "Running for package id: " << package_id;
      if (!DeletePerUserDirectories(package_id))
        return -1;
    }
  }
  return 0;
}
