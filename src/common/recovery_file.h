// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_RECOVERY_FILE_H_
#define COMMON_RECOVERY_FILE_H_

#include <boost/filesystem/path.hpp>

#include <cstdio>
#include <string>

#include "common/request.h"

namespace common_installer {

namespace recovery {

/**
 * Responsible for managing recovery file.
 *
 * It dumps information to recover file regarding current status of handled
 * request.
 */
class RecoveryFile {
 public:
  /**
   * Creates new RecoveryFile object for given file
   *
   * \param path path to the package/id
   *
   * \return new RecoveryFile object
   */
  static std::unique_ptr<RecoveryFile> CreateRecoveryFileForPath(
      const boost::filesystem::path& path);

  /**
   * Opens RecoveryFile object for given request
   *
   * \param path path to the package/id
   *
   * \return new RecoveryFile object
   */
  static std::unique_ptr<RecoveryFile> OpenRecoveryFileForPath(
      const boost::filesystem::path& path);

  /** Desctructor */
  __attribute__ ((visibility ("default"))) ~RecoveryFile();

  /** Detaching object from given recovery file */
  __attribute__ ((visibility ("default"))) void Detach();

  /**
   * Checks if object is detached from file
   *
   * \return true if detached
   *
   */
  __attribute__ ((visibility ("default"))) bool is_detached() const;

  /**
   * setter for unpacked dir
   *
   * \param unpacked_dir new unpacked_dir value
   */
  __attribute__ ((visibility ("default"))) void set_unpacked_dir(const boost::filesystem::path& unpacked_dir);

  /**
   * setter for pkgid
   *
   * \param pkgid new pkgid value
   */
  __attribute__ ((visibility ("default"))) void set_pkgid(const std::string& pkgid);

  /**
   * setter for request type
   *
   * \param new request type value
   */
  __attribute__ ((visibility ("default"))) void set_type(RequestType type);

  /**
   * getter for unpacked dir
   *
   * \return current unpacked_dir
   */
  __attribute__ ((visibility ("default"))) const boost::filesystem::path& unpacked_dir() const;

  /**
   * getter for pkgid
   *
   * * \return current pkgid
   */
  __attribute__ ((visibility ("default"))) const std::string& pkgid() const;

  /**
   * getter for request type
   *
   * \return current request type
   */
  __attribute__ ((visibility ("default"))) RequestType type() const;

  /**
   * Transaction of current RecoveryFile content into recovery file
   *
   * \return true if success
   */
  __attribute__ ((visibility ("default"))) bool WriteAndCommitFileContent();

 private:
  __attribute__ ((visibility ("default"))) RecoveryFile(const boost::filesystem::path& path, bool load);

  __attribute__ ((visibility ("default"))) bool ReadFileContent();

  RequestType type_;
  boost::filesystem::path unpacked_dir_;
  std::string pkgid_;

  boost::filesystem::path path_;
};

}  // namespace recovery
}  // namespace common_installer

#endif  // COMMON_RECOVERY_FILE_H_
