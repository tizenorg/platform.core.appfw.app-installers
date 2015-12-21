// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>
#include <package-manager.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <manifest_parser/utils/logging.h>
#include <tpk_manifest_handlers/package_handler.h>
#include <tpk_manifest_handlers/tpk_config_parser.h>

#include <iostream>

#include "common/request.h"

namespace bf = boost::filesystem;
namespace bpo = boost::program_options;
namespace ci = common_installer;

namespace {

const char kBackendDirectoryPath[] = "/etc/package-manager/backend/";

int InstallManifestOffline(const std::string& pkgid,
                           const std::string& type) {
  int pid = fork();
  if (pid == 0) {
    bf::path path(kBackendDirectoryPath);
    path /= type;
    const char* const argv[] = {
      path.c_str(),
      "-y",
      pkgid.c_str(),
      nullptr,
    };
    execv(argv[0], const_cast<char* const*>(argv));
    return -1;
  } else if (pid == -1) {
    LOG(ERROR) << "Failed to fork";
    return -1;
  } else {
    int status;
    waitpid(pid, &status, 0);
    return status;
  }
}

int InstallManifestOnline(const std::string& pkgid, const std::string& type) {
  pkgmgr_client* client = pkgmgr_client_new(PC_REQUEST);
  int ret = pkgmgr_client_direct_manifest_install(client, type.c_str(),
                                                  pkgid.c_str(), PM_QUIET,
                                                  nullptr, nullptr);
  pkgmgr_client_free(client);
  return ret == PKGMGR_R_OK ? 0 : -1;
}

}  // namespace

int main(int argc, char** argv) {
  bpo::options_description options("Allowed options");
  options.add_options()
      ("offline,o", "perform installation in offline mode")
      ("xml-path,x", bpo::value<std::string>()->required(), "xml package path")
      ("help,h", "display this help message");
  bpo::variables_map opt_map;
  try {
    bpo::store(bpo::parse_command_line(argc, argv, options), opt_map);
    if (opt_map.count("help")) {
      std::cerr << options << std::endl;
      return -1;
    }
    bpo::notify(opt_map);
  } catch (const std::exception& error) {
    std::cerr << error.what() << std::endl;
    return -1;
  }

  bf::path manifest_path(opt_map["xml-path"].as<std::string>());
  tpk::parse::TPKConfigParser parser;
  if (!parser.ParseManifest(manifest_path)) {
    LOG(ERROR) << "Failed to parse tizen manifest file: "
               << parser.GetErrorMessage();
    return 1;
  }
  auto package_info = std::static_pointer_cast<const tpk::parse::PackageInfo>(
      parser.GetManifestData(tpk::parse::PackageInfo::key()));
  if (!package_info) {
    LOG(ERROR) << "Failed to get package info";
    return 1;
  }
  std::string type = package_info->type();
  if (type.empty())
    type = "tpk";

  bool offline = opt_map.count("offline") != 0;
  if (offline) {
    return InstallManifestOffline(package_info->package(), type);
  } else {
    return InstallManifestOnline(package_info->package(), type);
  }
}
