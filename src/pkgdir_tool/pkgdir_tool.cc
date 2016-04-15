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
#include <string>
#include <utility>
#include <vector>

#include "common/shared_dirs.h"
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

enum class DirectoryOperation {
  NONE,
  CREATE_INTERNAL,
  DELETE
};

const char kCreateInternalMode[] = "create";
const char kDeleteMode[] = "delete";
const char kSinglePkgId[] = "pkgid";
const char kAllPkgIds[] = "allglobalpkgs";
const char kUserId[] = "user";

template<typename ... Arguments>
bool ExclusiveOptions(const bpo::variables_map& vm, Arguments... args) {
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
    LOG(ERROR) << exception.c_str();
    return false;
  }
  return true;
}

DirectoryOperation ParseDirectoryOptions(const bpo::variables_map& opt_map) {
  if (opt_map.count(kCreateInternalMode))
    return DirectoryOperation::CREATE_INTERNAL;

  if (opt_map.count(kDeleteMode))
    return DirectoryOperation::DELETE;

  return DirectoryOperation::NONE;
}

bpo::options_description CreateProgramOptions() {
  bpo::options_description options("Allowed options");
  options.add_options()
      (kCreateInternalMode, "create per user diretories for global package")
      (kDeleteMode, "delete per user diretories for global package")
      (kAllPkgIds, "install directories for all global applications")
      (kSinglePkgId, bpo::value<std::string>(), "package ID")
      (kUserId, bpo::value<uid_t>(), "user id");
  return options;
}

ci::PkgList GetPackageListFromArgs(const bpo::variables_map& opt_map) {
  bool allglobalpkgs = opt_map.count(kAllPkgIds) != 0;
  if (allglobalpkgs) return ci::CreatePkgInformationList();

  std::vector<std::string> pkgs;
  if (opt_map.count(kSinglePkgId)) {
    std::string pkgid = opt_map[kSinglePkgId].as<std::string>();
    pkgs.push_back(std::move(pkgid));
  }
  return ci::CreatePkgInformationList(getuid(), pkgs);
}

uid_t GetUserIdFromArgs(const bpo::variables_map& opt_map) {
  if (opt_map.count(kUserId))
    return opt_map[kUserId].as<uid_t>();
  return 0;
}

bool ParseCommandLine(int argc, char** argv,
                      const bpo::options_description& options,
                      bpo::variables_map* opt_map) {
  bpo::store(bpo::parse_command_line(argc, argv, options), *opt_map);
  if (!ExclusiveOptions(*opt_map, kCreateInternalMode, kDeleteMode)) {
    LOG(ERROR) << "Could not parse arguments: incorrect directory operation";
    return false;
  }

  if (!ExclusiveOptions(*opt_map, kSinglePkgId, kAllPkgIds)) {
    LOG(ERROR) << "Could not parse arguments: incorrect pkgid or pkgid\'s";
    return false;
  }
  bpo::notify(*opt_map);
  return true;
}

}  // namespace


int main(int argc, char** argv) {
  bpo::options_description options = CreateProgramOptions();
  bpo::variables_map opt_map;
  if (!ParseCommandLine(argc, argv, options, &opt_map)) return -1;

  assert(setuid(0) == 0);

  auto dir_operation = ParseDirectoryOptions(opt_map);
  auto pkgs = GetPackageListFromArgs(opt_map);
  auto uid = GetUserIdFromArgs(opt_map);

  for (auto& p : pkgs) {
    switch (dir_operation) {
      case DirectoryOperation::CREATE_INTERNAL: {
        LOG(DEBUG) << "Running directory creation for package id: " << p.pkg_id;
        if (uid != 0) {
          ci::PerformInternalDirectoryCreationForUser(uid,
                                                      p.pkg_id,
                                                      p.author_id);
          ci::SetPackageDirectorySmackRulesForUser(uid,
                                                   p.pkg_id,
                                                   p.author_id,
                                                   p.api_version);
        } else {
          ci::PerformInternalDirectoryCreationForAllUsers(p.pkg_id,
                                                          p.author_id);
          ci::SetPackageDirectorySmackRulesForAllUsers(p.pkg_id,
                                                       p.author_id,
                                                       p.api_version);
        }
      }
      break;
      case DirectoryOperation::DELETE: {
        LOG(DEBUG) << "Running directory deletion for package id: " << p.pkg_id;
        ci::PerformDirectoryDeletionForAllUsers(p.pkg_id);
      }
      break;
      default:
        break;
    }
  }
  return 0;
}
