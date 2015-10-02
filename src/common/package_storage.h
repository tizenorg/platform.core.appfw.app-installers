// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_PACKAGE_STORAGE_H_
#define COMMON_PACKAGE_STORAGE_H_

#include <boost/filesystem/path.hpp>

#include <sys/types.h>

#include <memory>
#include <string>

#include "common/request.h"

namespace common_installer {

class PackageStorage {
 public:
  PackageStorage(RequestType type, const std::string& pkgid,
                 const boost::filesystem::path& application_root, uid_t uid)
      : type_(type),
        pkgid_(pkgid),
        application_root_(application_root),
        uid_(uid) { }
  virtual ~PackageStorage() { }
  /**
   * @brief Initialize
   *        Initializes newly created storage
   *
   * @param space_requirement package files which will indicate the space
   *        requirements of installation/uninstallation process. Pass the
   *        directory you want to install
   * @return success of operation
   */
  virtual bool Initialize(const boost::filesystem::path& space_requirement) = 0;
  /**
   * @brief IsInstalled
   *        Checks if given package id is installed by this storage type.
   *
   * @return true if storage knows about app
   */
  virtual bool IsRequestedPackageInstalled() const = 0;
  /**
   * @brief path
   *        Returns valid package path which can be used in operation after
   *        package storage is successfully initialized.
   *
   * @return package path
   */
  boost::filesystem::path path() const;
  /**
   * @brief Commit
   *        Commits all operation of filesystem if necessary. This method should
   *        be called at the end of installation when it is successfull.
   * @return success of operation
   */
  virtual bool Commit() = 0;
  /**
   * @brief Abort
   *        Abort all operation of filesystem if necessary according to mode
   *        of installation. This method should be called at the end of
   *        installation when it failed.
   * @return success of operation
   */
  virtual bool Abort() = 0;

  /**
   * @brief StorageName
   *        Returns value that will be inserted in package manager database to
   *        identify which storage was used in case of update and deinstallation
   * @return
   */
  virtual const std::string& StorageName() const = 0;

 protected:
  RequestType type_;
  std::string pkgid_;
  boost::filesystem::path application_root_;
  uid_t uid_;
};

std::unique_ptr<PackageStorage> AcquirePackageStorage(RequestType type,
    const boost::filesystem::path& application_root, const std::string& pkgid,
    const boost::filesystem::path& space_requirement,
    const char* installed_storage, const char* preference, uid_t uid);

}  // namespace common_installer

#endif  // COMMON_PACKAGE_STORAGE_H_
