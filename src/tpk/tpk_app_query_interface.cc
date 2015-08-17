// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "tpk/tpk_app_query_interface.h"

#include <unistd.h>
#include <sys/types.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>

#include <memory>
#include <string>

#include "common/pkgmgr_registration.h"
#include "common/utils/file_util.h"
#include "common/utils/logging.h"
#include "tpk/xml_parser/xml_parser.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;
namespace ci = common_installer;
namespace xp = xml_parser;

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
  xp::XmlParser parser;
  std::unique_ptr<xp::XmlTree> tree(parser.ParseAndGetNewTree(
      manifest_path.c_str()));
  if (!tree) {
    bf::remove_all(tmp_path, code);
    return {};
  }
  xp::XmlElement* manifest = tree->GetRootElement();
  if (!manifest) {
    bf::remove_all(tmp_path, code);
    return {};
  }
  std::string pkg_id = manifest->attr("package");
  bf::remove_all(tmp_path, code);
  return pkg_id;
}

}  // namespace

namespace tpk {

bool TpkAppQueryInterface::IsAppInstalledByArgv(int argc, char** argv) {
  std::string path = GetInstallationPackagePath(argc, argv);
  if (path.empty()) {
    // not the installaton
    return false;
  }
  std::string pkg_id = GetPkgIdFromPath(path);
  if (pkg_id.empty())
    return false;
  return ci::IsPackageInstalled(pkg_id, getuid());
}

}  // namespace tpk

