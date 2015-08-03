// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_RECOVERY_FILE_H_
#define COMMON_RECOVERY_FILE_H_

#include <boost/filesystem/path.hpp>

#include <cstdio>
#include <string>

#include "common/pkgmgr_interface.h"
#include "common/request_type.h"

namespace common_installer {

namespace recovery {

class RecoveryFile {
 public:
  static std::unique_ptr<RecoveryFile> CreateRecoveryFileForPath(
      const boost::filesystem::path& path);
  static std::unique_ptr<RecoveryFile> OpenRecoveryFileForPath(
      const boost::filesystem::path& path);
  ~RecoveryFile();
  void Detach();
  bool is_detached() const;

  void set_unpacked_dir(const boost::filesystem::path& unpacked_dir);
  void set_pkgid(const std::string& pkgid);
  void set_type(RequestType type);
  const boost::filesystem::path& unpacked_dir() const;
  const std::string& pkgid() const;
  RequestType type() const;

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
