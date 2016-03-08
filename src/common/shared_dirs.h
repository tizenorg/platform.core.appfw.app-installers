// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_SHARED_DIRS_H_
#define COMMON_SHARED_DIRS_H_

#include <boost/filesystem/path.hpp>

#include <string>
#include <vector>

namespace common_installer {

struct pkg_info {
  pkg_info(std::string pkg_id_, std::string api_version_, std::string author_) :
    pkg_id(pkg_id_), api_version(api_version_), author_id(author_) {}

  std::string pkg_id;
  std::string api_version;
  std::string author_id;
};

using pkg_list = std::vector<pkg_info>;

/**
 * \brief Performs a creation of directories for specific user in internal storage
 *
 * \param pkgid id of package
 * \param author_id id of author
 * \param api_version api version
 *
 * \return true if succeed, false otherwise
 *
 */
bool PerformInternalDirectoryCreationForUser(const char* user_name,
                                             const std::string& pkgid,
                                             const std::string& author_id,
                                             const std::string& api_version);

/**
 * \brief Performs a creation of directories for specific user in external storage
 *
 * \param pkgid id of package
 * \param author_id id of author
 * \param api_version api version
 *
 * \return true if succeed, false otherwise
 *
 */
bool PerformExternalDirectoryCreationForUser(const char* user_name,
                                             const std::string& pkgid,
                                             const std::string& author_id,
                                             const std::string& api_version);

/**
 * \brief Performs a creation of directories in internal storage
 *
 * \param pkgid id of package
 * \param author_id id of author
 * \param api_version api version
 *
 * \return true if succeed, false otherwise
 *
 */
bool PerformInternalDirectoryCreationForAllUsers(const std::string& pkgid,
                                                 const std::string& author_id,
                                                 const std::string& apiversion);

/**
 * \brief Performs a creation of directories in external storage (eg. SD card)
 *
 * \param pkgid id of package
 * \param author_id id of author
 * \param api_version api version
 *
 * \return true if succeed, false otherwise
 *
 */
bool PerformExternalDirectoryCreationForAllUsers(const std::string& pkgid,
                                                 const std::string& author_id,
                                                 const std::string& apiversion);

/**
 * \brief Performs deletion of directories
 *
 * \param pkg_path package path
 *
 * \return true if succeed, false otherwise
 *
 */
bool PerformDirectoryDeletion(const std::string& pkgid);

/**
 * \brief Helper function fetching information about packages
 *
 * \param pkgs  list of packages requested to fetch information about. If list
 *              is empty, all possible pkg id's are considered.
 *
 * \return pkg_list list containing information about requested packages
 *
 */
pkg_list CreatePkgInformationList(const std::vector<std::string>& pkgs =
    std::vector<std::string>());

}  // namespace common_installer

#endif  // COMMON_SHARED_DIRS_H_
