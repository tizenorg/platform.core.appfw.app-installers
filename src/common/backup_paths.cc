// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/backup_paths.h"

#include <pwd.h>
#include <tzplatform_config.h>

namespace bf = boost::filesystem;

namespace {

const int32_t kPWBufSize = sysconf(_SC_GETPW_R_SIZE_MAX);
const char kImageDir[] = ".image";
const char kBckExtension[] = ".bck";
const char kExternalStorageDirPrefix[] = "SDCardA1";

boost::filesystem::path GetBackupPath(
    const boost::filesystem::path& pkg_path) {
  bf::path backup_path = pkg_path;
  backup_path += kBckExtension;
  return backup_path;
}

std::string GetUserNameForUID(uid_t uid) {
  struct passwd pwd;
  struct passwd *pwd_result;
  char buf[kPWBufSize];
  int ret = getpwuid_r(uid, &pwd, buf, sizeof(buf), &pwd_result);
  if (ret != 0 || pwd_result == nullptr)
    return {};
  return pwd.pw_name;
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
  return kBckExtension;
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

boost::filesystem::path GetExternalCardPath() {
  return bf::path(tzplatform_mkpath(TZ_SYS_MEDIA, kExternalStorageDirPrefix));
}

boost::filesystem::path GetExternalTepPath(RequestMode request_mode,
                                           uid_t uid) {
  bf::path result = GetExternalCardPath() / "tep";
  if (request_mode == RequestMode::USER)
    result /= GetUserNameForUID(uid);
  return result;
}

boost::filesystem::path GetInternalTepPath(
    const boost::filesystem::path& pkg_path) {
  return pkg_path / "tep";
}

}  // namespace common_installer
