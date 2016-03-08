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

bool ValidateTizenPackageId(const std::string& id);

pkg_list GetAllGlobalApps();

pkg_list GetPkg(const std::string& pkgid);

bool SetPackageDirectorySmackRules(const boost::filesystem::path& base_dir,
                                   const std::string& pkgid,
                                   const std::string& author_id,
                                   const std::string& api_version,
                                   uid_t uid);

bool SetPackageDirectoryOwnerAndPermissions(
    const boost::filesystem::path& subpath, uid_t uid, gid_t gid);

bool CreateDirectories(const boost::filesystem::path& app_dir,
                       const std::string& pkgid,
                       const std::string& author_id,
                       const std::string& api_version,
                       uid_t uid, gid_t gid);

bool CreatePerUserDirectories(const std::string& pkgid,
    const std::string& author_id, const std::string& api_version,
    const std::string& apps_prefix = std::string());

bool CreatePerUserExternalStorageDirectories(const std::string& pkgid,
    const std::string& author_id, const std::string& api_version);

bool CreateSkelDirectories(const std::string& pkgid);

bool DeleteDirectories(const boost::filesystem::path& app_dir,
                       const std::string& pkgid);

bool DeletePerUserDirectories(const std::string& pkgid);

bool DeleteSkelDirectories(const std::string& pkgid);

bool PerformDirectoryCreation(const std::string& pkgid,
    const std::string& author_id, const std::string& api_version);

bool PerformExternalDirectoryCreation(const std::string& pkgid,
    const std::string& author_id, const std::string& api_version);

bool PerformDirectoryDeletion(const std::string& pkgid);


/**
 * \brief Helper function for getting backup path (used for recovery)
 *        based on package path
 *
 * \param pkg_path package path
 *
 * \return backup path
 *
 * boost::filesystem::path GetBackupPathForPackagePath(
    const boost::filesystem::path& pkg_path);
 */

}  // namespace common_installer

#endif  // COMMON_SHARED_DIRS_H_
