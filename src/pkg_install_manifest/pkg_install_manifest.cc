// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include <boost/filesystem/path.hpp>
#include <package-manager.h>

#include <manifest_parser/utils/logging.h>
#include <tpk_manifest_handlers/package_handler.h>
#include <tpk_manifest_handlers/tpk_config_parser.h>

#include "common/request.h"

namespace bf = boost::filesystem;
namespace ci = common_installer;

int main(int argc, char** argv) {
  if (argc != 2) {
    LOG(ERROR) << "Invalid usage. Try: pkg-install-manifest [path_to_manifest]";
    return 1;
  }
  bf::path manifest_path(argv[1]);
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
  const std::string& pkgid = package_info->package();
  pkgmgr_client* client = pkgmgr_client_new(PC_REQUEST);
  int ret = pkgmgr_client_direct_manifest_install(client, type.c_str(),
                                                  pkgid.c_str(), PM_QUIET,
                                                  nullptr, nullptr);
  pkgmgr_client_free(client);
  return ret == PKGMGR_R_OK;
}
