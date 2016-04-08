// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_BACKUP_PATHS_H_
#define COMMON_BACKUP_PATHS_H_

#include <boost/filesystem/path.hpp>

#include <string>

namespace common_installer {

/**
 * \brief Helper function for getting backup path (used for recovery)
 *        based on package path
 *
 * \param pkg_path package path
 *
 * \return backup path
 */
boost::filesystem::path GetBackupPathForPackagePath(
    const boost::filesystem::path& pkg_path);

/**
 * \brief Helper function for getting backup path (used for recovery)
 *        based on manifest file path
 *
 * \param manifest_path path to manifest file
 *
 * \return backup path
 */
boost::filesystem::path GetBackupPathForManifestFile(
    const boost::filesystem::path& manfest_path);

/**
 * \brief Helper function for getting backup path (used for recovery)
 *        based on icon path
 *
 * \param icon_path path to icon file
 *
 * \return backup path
 */
boost::filesystem::path GetBackupPathForIconFile(
    const boost::filesystem::path& icon_path);

/**
 * @brief GetIconFileBackupExtension
 * @return extension for backup icon files
 */
std::string GetIconFileBackupExtension();

/**
 * @brief GetZipPackageLocation
 *        Returns location where zip package file must be copied during package
 *        installation.
 *
 * @param pkg_path package path for which zip package location is being obtained
 *
 * @return full path of zip package location
 */
boost::filesystem::path GetZipPackageLocation(
    const boost::filesystem::path& pkg_path);

/**
 * @brief GetMountLocation
 *        Returns the mount point of zip package. This location is used to
 *        mount package.
 *
 * @param pkg_path package path for which zip mount location is being obtained
 *
 * @return full path of package mount point
 */
boost::filesystem::path GetMountLocation(
    const boost::filesystem::path& pkg_path);

}  // namespace common_installer

#endif  // COMMON_BACKUP_PATHS_H_
