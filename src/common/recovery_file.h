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
  ~RecoveryFile();

  /** Detaching object from given recovery file */
  void Detach();

  /**
   * Checks if object is detached from file
   *
   * \return true if detached
   *
   */
  bool is_detached() const;

  /**
   * setter for unpacked dir
   *
   * \param unpacked_dir new unpacked_dir value
   */
  void set_unpacked_dir(boost::filesystem::path unpacked_dir);

  /**
   * setter for pkgid
   *
   * \param pkgid new pkgid value
   */
  void set_pkgid(std::string pkgid);

  /**
   * setter for request type
   *
   * \param new request type value
   */
  void set_type(RequestType type);

  /**
   * getter for unpacked dir
   *
   * \return current unpacked_dir
   */
  const boost::filesystem::path& unpacked_dir() const;

  /**
   * getter for pkgid
   *
   * * \return current pkgid
   */
  const std::string& pkgid() const;

  /**
   * getter for request type
   *
   * \return current request type
   */
  RequestType type() const;

  /**
   * Transaction of current RecoveryFile content into recovery file
   *
   * \return true if success
   */
  bool WriteAndCommitFileContent();

 private:
  RecoveryFile(const boost::filesystem::path& path, bool load);

  bool ReadFileContent();

  RequestType type_;
  boost::filesystem::path unpacked_dir_;
  std::string pkgid_;

  boost::filesystem::path path_;
};

}  // namespace recovery
}  // namespace common_installer

#endif  // COMMON_RECOVERY_FILE_H_
