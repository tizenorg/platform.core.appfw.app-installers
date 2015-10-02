// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_PACKAGE_STORAGE_H_
#define COMMON_PACKAGE_STORAGE_H_

#include <boost/filesystem/path.hpp>

#include <memory>
#include <string>

#include "common/request.h"

namespace common_installer {

class PackageStorage {
 public:
  PackageStorage(RequestType type, const std::string& pkgid,
                 const boost::filesystem::path& application_root)
      : type_(type),
        pkgid_(pkgid),
        application_root_(application_root) { }
  virtual ~PackageStorage() { }
  /**
   * @brief Initialize
   *        Initializes newly created storage
   *
   * @param space_requirement package files which will indicate the space
   *        requirements of installation/uninstallation process
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

 protected:
  RequestType type_;
  std::string pkgid_;
  boost::filesystem::path application_root_;
};

std::unique_ptr<PackageStorage> CreatePackageStorage(RequestType type,
    const boost::filesystem::path& application_root, const std::string& pkgid,
    const boost::filesystem::path& space_requirement,
    const char* install_location);

}  // namespace common_installer

#endif  // COMMON_PACKAGE_STORAGE_H_
