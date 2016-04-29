// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_SHARED_DIRS_H_
#define COMMON_SHARED_DIRS_H_

#include <boost/filesystem/path.hpp>

#include <string>
#include <vector>

namespace common_installer {

struct PkgInfo {
  PkgInfo(std::string pkg_id_, std::string api_version_, std::string author_)
    : pkg_id(pkg_id_),
      api_version(api_version_),
      author_id(author_) {}

  std::string pkg_id;
  std::string api_version;
  std::string author_id;
};

using PkgList = std::vector<PkgInfo>;

/**
 * \brief Performs a creation of directories for specific user in internal storage
 *
 * \param pkgid id of package
 * \param author_id id of author
 * \param create_skel_directories flag
 *
 * \return true if succeed, false otherwise
 *
 */
bool PerformInternalDirectoryCreationForUser(uid_t uid,
                                             const std::string& pkgid,
                                             const std::string& author_id);

/**
 * \brief Performs a creation of directories for specific user in external storage
 *
 * \param pkgid id of package
 * \param author_id id of author
 * \param create_skel_directories flag
 *
 * \return true if succeed, false otherwise
 *
 */
bool PerformExternalDirectoryCreationForUser(uid_t uid,
                                             const std::string& pkgid,
                                             const std::string& author_id);

/**
 * \brief Performs a creation of directories in internal storage
 *
 * \param pkgid id of package
 * \param author_id id of author
 * \param create_skel_directories flag
 *
 * \return true if succeed, false otherwise
 *
 */
bool PerformInternalDirectoryCreationForAllUsers(const std::string& pkgid,
                                                 const std::string& author_id);

/**
 * \brief Performs a creation of directories in external storage (eg. SD card)
 *
 * \param pkgid id of package
 * \param author_id id of author
 * \param create_skel_directories flag
 *
 * \return true if succeed, false otherwise
 *
 */
bool PerformExternalDirectoryCreationForAllUsers(const std::string& pkgid,
                                                 const std::string& author_id);

/**
 * \brief Performs deletion of directories
 *
 * \param pkg_path package path
 *
 * \return true if succeed, false otherwise
 *
 */
bool PerformDirectoryDeletionForAllUsers(const std::string& pkgid);

/**
 * \brief Helper function fetching information about packages
 *
 * \param pkgs  list of packages requested to fetch information about. If list
 *              is empty, all possible pkg id's are considered.
 *
 * \return pkg_list list containing information about requested packages
 *
 */
PkgList CreatePkgInformationList(uid_t uid = getuid(),
                                 const std::vector<std::string>& pkgs =
    std::vector<std::string>());

/**
 * \brief Create skeleton directories for package
 *
 * \param pkgid package id
 *
 * \return bool true if succeed, false otherwise
 *
 */
bool CreateSkelDirectories(const std::string& pkgid);

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
 * \brief Request to copy per-user directories
 *
 * \param pkgid package id
 *
 * \return bool true if succeed, false otherwise
 *
 */
bool RequestCopyUserDirectories(const std::string& pkgid);

/**
 * \brief Request to delete per-user directories
 *
 * \param pkgid package id
 *
 * \return bool true if succeed, false otherwise
 *
 */
bool RequestDeleteUserDirectories(const std::string& pkgid);

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

}  // namespace common_installer

#endif  // COMMON_SHARED_DIRS_H_
