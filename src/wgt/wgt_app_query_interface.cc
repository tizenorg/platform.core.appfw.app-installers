// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/wgt_app_query_interface.h"

#include <unistd.h>
#include <sys/types.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>

#include <manifest_handlers/application_manifest_constants.h>
#include <manifest_handlers/tizen_application_handler.h>
#include <manifest_handlers/widget_handler.h>
#include <manifest_parser/manifest_parser.h>
#include <manifest_parser/utils/logging.h>

#include <memory>
#include <string>
#include <vector>

#include "common/pkgmgr_registration.h"
#include "common/request.h"
#include "common/utils/file_util.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;
namespace ci = common_installer;

namespace {

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

std::string GetPkgIdFromPath(const std::string& path) {
  bf::path tmp_path = common_installer::GenerateTmpDir("/tmp");
  bs::error_code code;
  bf::create_directories(tmp_path, code);
  if (code)
    return {};
  if (!common_installer::ExtractToTmpDir(path.c_str(), tmp_path,
      "config.xml")) {
    bf::remove_all(tmp_path, code);
    return {};
  }
  bf::path config_path = tmp_path / "config.xml";
  std::vector<parser::ManifestHandler*> handlers = {
    new wgt::parse::WidgetHandler(),
    new wgt::parse::TizenApplicationHandler()
  };
  std::unique_ptr<parser::ManifestHandlerRegistry> registry(
      new parser::ManifestHandlerRegistry(handlers));
  std::unique_ptr<parser::ManifestParser> parser(
      new parser::ManifestParser(std::move(registry)));
  if (!parser->ParseManifest(config_path)) {
    bf::remove_all(tmp_path, code);
    return {};
  }
  auto info = std::static_pointer_cast<const wgt::parse::TizenApplicationInfo>(
      parser->GetManifestData(
          wgt::application_widget_keys::kTizenApplicationKey));
  if (!info) {
    bf::remove_all(tmp_path, code);
    return {};
  }
  std::string pkg_id = info->package();

  bf::remove_all(tmp_path, code);
  return pkg_id;
}

}  // namespace

namespace wgt {

bool WgtAppQueryInterface::IsAppInstalledByArgv(int argc, char** argv) {
  std::string path = GetInstallationPackagePath(argc, argv);
  if (path.empty()) {
    // not the installaton
    return false;
  }
  std::string pkg_id = GetPkgIdFromPath(path);
  if (pkg_id.empty())
    return false;
  return ci::IsPackageInstalled(pkg_id, ci::GetRequestMode());
}

}  // namespace wgt
