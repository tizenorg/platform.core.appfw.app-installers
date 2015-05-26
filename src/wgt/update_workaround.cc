// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/update_workaround.h"

#include <unistd.h>
#include <sys/types.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>

#include <manifest_handlers/application_manifest_constants.h>
#include <manifest_handlers/tizen_application_handler.h>
#include <manifest_handlers/widget_handler.h>
#include <manifest_parser/manifest_parser.h>

#include <pkgmgr-info.h>

#include <cstring>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "utils/file_util.h"
#include "utils/logging.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace {

std::pair<std::string, int> GetInstallationPackagePath(int argc, char** argv) {
  int index = -1;
  std::string path;
  for (unsigned i = 0; i < argc; ++i) {
    if (!strcmp(argv[i], "-i")) {
      index = i;
      if (i + 1 < argc) {
        path = argv[i + 1];
      }
      break;
    }
  }
  return std::make_pair(path, index);
}

std::string GetAppIdFromPath(const std::string& path) {
  bf::path tmp_path = common_installer::utils::GenerateTmpDir("/tmp");
  bs::error_code code;
  bf::create_directories(tmp_path, code);
  if (code)
    return {};
  if (!common_installer::utils::ExtractToTmpDir(path.c_str(), tmp_path,
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

bool IsPackageInstalled(const std::string& pkg_id) {
  pkgmgrinfo_pkginfo_h handle;
  int ret = pkgmgrinfo_pkginfo_get_usr_pkginfo(pkg_id.c_str(), getuid(),
                                               &handle);
  if (ret != PMINFO_R_OK)
    return false;
  pkgmgrinfo_pkginfo_destroy_pkginfo(handle);
  return true;
}

void OverwriteArgvForUpdate(int index, char** argv) {
  // overwriting argv to fake update
  argv[index][1] = 'a';
}

}  // namespace

namespace workaround {

void OverwriteArgvIfNeeded(int argc, char** argv) {
  std::string path;
  int index;
  std::tie(path, index) =
      GetInstallationPackagePath(argc, argv);
  if (path.empty()) {
    // not the installaton
    return;
  }
  std::string pkg_id = GetAppIdFromPath(path);
  if (pkg_id.empty())
    return;
  bool should_hack = IsPackageInstalled(pkg_id);
  if (should_hack) {
    LOG(INFO) << "Update detected - hacking argv to update installation";
    OverwriteArgvForUpdate(index, argv);
  }
}

}  // namespace workaround
