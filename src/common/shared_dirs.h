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
 * \param api_version api version
 * \param create_skel_directories flag
 *
 * \return true if succeed, false otherwise
 *
 */
bool PerformInternalDirectoryCreationForUser(uid_t uid,
                                             const std::string& pkgid,
                                             const std::string& author_id,
                                             const std::string& api_version,
                                             bool create_skel_directories);

/**
 * \brief Performs a creation of directories for specific user in external storage
 *
 * \param pkgid id of package
 * \param author_id id of author
 * \param api_version api version
 * \param create_skel_directories flag
 *
 * \return true if succeed, false otherwise
 *
 */
bool PerformExternalDirectoryCreationForUser(uid_t uid,
                                             const std::string& pkgid,
                                             const std::string& author_id,
                                             const std::string& api_version,
                                             bool create_skel_directories);

/**
 * \brief Performs a creation of directories in internal storage
 *
 * \param pkgid id of package
 * \param author_id id of author
 * \param api_version api version
 * \param create_skel_directories flag
 *
 * \return true if succeed, false otherwise
 *
 */
bool PerformInternalDirectoryCreationForAllUsers(const std::string& pkgid,
                                                 const std::string& author_id,
                                                 const std::string& apiversion,
                                                 bool create_skel_directories);

/**
 * \brief Performs a creation of directories in external storage (eg. SD card)
 *
 * \param pkgid id of package
 * \param author_id id of author
 * \param api_version api version
 * \param create_skel_directories flag
 *
 * \return true if succeed, false otherwise
 *
 */
bool PerformExternalDirectoryCreationForAllUsers(const std::string& pkgid,
                                                 const std::string& author_id,
                                                 const std::string& apiversion,
                                                 bool create_skel_directories);

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
 * \brief Performs deletion of directories
 *
 * \param uid package path
 * \param pkg_path package path
 * \param pkg_id package path
 * \param author_id package path
 * \param api_version package path
 *
 *
 * \return true if succeed, false otherwise
 *
 */
bool SetPackageDirectorySmackRulesForUser(uid_t uid,
                                          const std::string& pkg_path,
                                          const std::string& pkg_id,
                                          const std::string& author_id,
                                          const std::string& api_version);

/**
 * \brief Performs deletion of directories
 *
 * \param pkg_path package path
 * \param pkg_id package path
 * \param author_id package path
 * \param api_version package path
 *
 * \return true if succeed, false otherwise
 *
 */
bool SetPackageDirectorySmackRulesForAllUsers(const std::string& pkg_path,
                                              const std::string& pkg_id,
                                              const std::string& author_id,
                                              const std::string& api_version);

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
