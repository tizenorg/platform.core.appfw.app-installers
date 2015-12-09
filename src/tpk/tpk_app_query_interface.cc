// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "tpk/tpk_app_query_interface.h"

#include <unistd.h>
#include <sys/types.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>
#include <tpk_manifest_handlers/application_manifest_constants.h>
#include <tpk_manifest_handlers/package_handler.h>
#include <tpk_manifest_handlers/tpk_config_parser.h>

#include <memory>
#include <string>

#include "common/pkgmgr_registration.h"
#include "common/request.h"
#include "common/utils/file_util.h"
#include "common/utils/logging.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;
namespace ci = common_installer;

namespace {

const char kManifestFileName[] = "tizen-manifest.xml";

std::string GetInstallationPackagePath(int argc, char** argv) {
  std::string path;
  for (int i = 0; i < argc; ++i) {
    if (!strcmp(argv[i], "-i")) {
      if (i + 1 < argc) {
        path = argv[i + 1];
        break;
      }
    }
  }
  return path;
}

std::string GetXmlPath(int argc, char** argv) {
  std::string path;
  for (int i = 0; i < argc; ++i) {
    if (!strcmp(argv[i], "-x")) {
      if (i + 1 < argc) {
        path = argv[i + 1];
        break;
      }
    }
  }
  return path;
}

std::string GetPkgIdFromXml(const std::string&path) {
  bf::path xml_path(path);

  return xml_path.stem().string();
}

std::string GetPkgIdFromPath(const std::string& path) {
  bf::path tmp_path = common_installer::GenerateTmpDir("/tmp");
  bs::error_code code;
  bf::create_directories(tmp_path, code);
  if (code)
    return {};
  if (!common_installer::ExtractToTmpDir(path.c_str(), tmp_path,
      kManifestFileName)) {
    bf::remove_all(tmp_path, code);
    return {};
  }
  bf::path manifest_path = tmp_path / kManifestFileName;
  if (!bf::exists(manifest_path)) {
    bf::remove_all(tmp_path, code);
    return {};
  }

  tpk::parse::TPKConfigParser parser;
  if (!parser.ParseManifest(manifest_path))
    return {};
  auto package_info = std::static_pointer_cast<const tpk::parse::PackageInfo>(
      parser.GetManifestData(tpk::application_keys::kManifestKey));
  if (!package_info)
    return {};
  std::string pkg_id = package_info->package();
  bf::remove_all(tmp_path, code);
  return pkg_id;
}

}  // namespace

namespace tpk {

bool TpkAppQueryInterface::IsAppInstalledByArgv(int argc, char** argv) {
  std::string path = GetInstallationPackagePath(argc, argv);
  std::string pkg_id;
  if (path.empty()) {
    // check if it is manifest direct install
    path = GetXmlPath(argc, argv);
    if (path.empty())
      return false;

    pkg_id = GetPkgIdFromXml(path);
  } else {
    pkg_id = GetPkgIdFromPath(path);
  }

  if (pkg_id.empty())
    return false;
  return ci::IsPackageInstalled(pkg_id, ci::GetRequestMode());
}

}  // namespace tpk

