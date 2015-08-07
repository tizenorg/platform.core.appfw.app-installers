// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_BACKUP_PATHS_H_
#define COMMON_BACKUP_PATHS_H_

#include <boost/filesystem/path.hpp>

namespace common_installer {

boost::filesystem::path GetBackupPathForPackagePath(
    const boost::filesystem::path& pkg_path);
boost::filesystem::path GetBackupPathForManifestFile(
    const boost::filesystem::path& manfest_path);
boost::filesystem::path GetBackupPathForIconFile(
    const boost::filesystem::path& icon_path);

}  // namespace common_installer

#endif  // COMMON_BACKUP_PATHS_H_
