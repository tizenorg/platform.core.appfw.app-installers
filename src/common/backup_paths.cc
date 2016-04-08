// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/backup_paths.h"

#include <tzplatform_config.h>

namespace bf = boost::filesystem;

namespace {

const char kImageDir[] = ".image";

boost::filesystem::path GetBackupPath(
    const boost::filesystem::path& pkg_path) {
  bf::path backup_path = pkg_path;
  backup_path += ".bck";
  return backup_path;
}

}  // namespace

namespace common_installer {

boost::filesystem::path GetBackupPathForPackagePath(
    const boost::filesystem::path& pkg_path) {
  return GetBackupPath(pkg_path);
}

boost::filesystem::path GetBackupPathForManifestFile(
    const boost::filesystem::path& manfest_path) {
  return GetBackupPath(manfest_path);
}

boost::filesystem::path GetBackupPathForIconFile(
    const boost::filesystem::path& icon_path) {
  return GetBackupPath(icon_path);
}

std::string GetIconFileBackupExtension() {
  return ".bck";
}

boost::filesystem::path GetBackupPathForZipFile(const bf::path& zip_path) {
  return GetBackupPath(zip_path);
}

boost::filesystem::path GetMountLocation(const bf::path& pkg_path) {
  return pkg_path / ".pkg";
}

boost::filesystem::path GetZipPackageLocation(
    const boost::filesystem::path& pkg_path,
    const std::string& pkgid) {
  return pkg_path / kImageDir / pkgid;
}

}  // namespace common_installer
