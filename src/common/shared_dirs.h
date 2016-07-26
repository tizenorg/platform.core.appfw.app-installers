// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_SHARED_DIRS_H_
#define COMMON_SHARED_DIRS_H_

#include <boost/filesystem/path.hpp>
#include <pkgmgrinfo_basic.h>

#include <string>
#include <vector>

namespace common_installer {

/**
 * \brief Performs a creation of directories for specific user in external storage
 *
 * \param pkgid id of package
 * \param create_skel_directories flag
 *
 * \return true if succeed, false otherwise
 *
 */
bool PerformExternalDirectoryCreationForUser(uid_t uid,
                                             const std::string& pkgid);

/**
 * \brief Performs a removal of directories for specific user in external storage
 * \param user id of user
 * \param pkgid id of package
 *
 * \return true if succeed, false otherwise
 */
bool PerformExternalDirectoryDeletionForUser(uid_t user,
                                            const std::string& pkgid);

/**
 * \brief Performs a creation of directories in external storage (eg. SD card)
 *
 * \param pkgid id of package
 * \param create_skel_directories flag
 *
 * \return true if succeed, false otherwise
 *
 */
bool PerformExternalDirectoryCreationForAllUsers(const std::string& pkgid);

/**
 * \brief Performs a removal of directories in external storage (eg. SD card)
 * \param pkgid id of package
 *
 * \return true if succeed, false otherwise
 */
bool PerformExternalDirectoryDeletionForAllUsers(const std::string& pkgid);

/**
 * \brief Create skeleton directories for package
 *
 * \param pkgid package id
 * \param api_version package api version
 * \param trusted signed package flag
 *
 * \return bool true if succeed, false otherwise
 *
 */
bool CreateSkelDirectories(const std::string& pkgid,
                           const std::string& api_version,
                           bool trusted);

/**
 * \brief Performs deletion of directories
 *
 * \param pkgid package id
 *
 * \return true if succeed, false otherwise
 *
 */
bool DeleteSkelDirectories(const std::string& pkgid);

/**
 * \brief Delete per-user directories
 *
 * \param pkgid package id
 *
 * \return true if succeed, false otherwise
 *
 */
bool DeleteUserDirectories(const std::string& pkgid);

/**
 * \brief Copy per-user directories
 *
 * \param pkgid package id
 *
 * \return bool true if succeed, false otherwise
 *
 */
bool CopyUserDirectories(const std::string& pkgid);

/**
 * \brief Returns path prefix for internal storage, typically '/home'
 *
 * \return path prefix
 *
 */
std::string GetDirectoryPathForInternalStorage();

/**
 * \brief Returns path prefix for external storage, typically sd card mount point
 *
 * \return path prefix
 *
 */
std::string GetDirectoryPathForExternalStorage();

/**
 * \brief Create Legacy directories
 *
 * \param uid user id
 * \param pkgid package id
 *
 * \return true if succeed, false otherwise
 *
 */
bool CreateLegacyDirectories(const std::string& pkgid);

/**
 * \brief Delete Legacy directories
 *
 * \param pkgid package id
 *
 * \return true if succeed, false otherwise
 *
 */
bool DeleteLegacyDirectories(uid_t uid, const std::string& pkgid);

/**
 * \brief Create symlinks for read-only files of global app for all users.
 *
 * \param pkgid package id
 *
 * \return true if succeed, false otherwise
 *
 */
bool CreateGlobalAppSymlinksForAllUsers(const std::string& pkgid);

/**
 * \brief Create symlinks for read-only files of global app for user.
 *
 * \param pkgid package id
 * \param uid user id
 *
 * \return true if succeed, false otherwise
 *
 */
bool CreateGlobalAppSymlinksForUser(const std::string& pkgid, uid_t uid);

/**
 * \brief Delete symlinks for read-only files of global app for all users.
 *
 * \param pkgid package id
 *
 * \return true if succeed, false otherwise
 *
 */
bool DeleteGlobalAppSymlinksForAllUsers(const std::string& pkgid);

/**
 * \brief Delete symlinks for read-only files of global app for user.
 *
 * \param pkgid package id
 * \param uid user id
 *
 * \return true if succeed, false otherwise
 *
 */
bool DeleteGlobalAppSymlinksForUser(const std::string& pkgid, uid_t uid);

}  // namespace common_installer

#endif  // COMMON_SHARED_DIRS_H_
