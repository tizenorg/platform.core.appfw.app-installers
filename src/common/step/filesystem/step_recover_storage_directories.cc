// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_recover_storage_directories.h"

#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>

#include "common/backup_paths.h"
#include "common/utils/file_util.h"

namespace {
const char kDataLocation[] = "data";
const char kSharedResLocation[] = "shared";
}  // namespace

namespace bf = boost::filesystem;

namespace common_installer {
namespace filesystem {

bool StepRecoverStorageDirectories::MoveAppStorage(
    const bf::path& in_src,
    const bf::path& in_dst,
    const char *key) {
  bf::path src = in_src / key;
  if (!bf::exists(src))
    return false;
  bf::path dst = in_dst / key;
  return common_installer::MoveDir(src, dst);
}

Step::Status StepRecoverStorageDirectories::RecoveryUpdate() {
  if (!context_->package_storage->path().empty()) {
    bf::path backup_path = common_installer::GetBackupPathForPackagePath(
        context_->package_storage->path());
    if (bf::exists(backup_path)) {
      MoveAppStorage(context_->package_storage->path(), backup_path,
                     kDataLocation);
      MoveAppStorage(context_->package_storage->path(), backup_path,
                     kSharedResLocation);
    }
  }
  return Status::OK;
}
}  // namespace filesystem
}  // namespace common_installer

