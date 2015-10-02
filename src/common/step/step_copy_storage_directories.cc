// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_copy_storage_directories.h"

#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>

#include <string>

#include "common/backup_paths.h"
#include "utils/file_util.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace {

const char kDataLocation[] = "data";
const char kSharedLocation[] = "shared";

}  // namespace

namespace common_installer {
namespace filesystem {

bool StepCopyStorageDirectories::MoveAppStorage(
    const bf::path& in_src,
    const bf::path& in_dst,
    const char *key) {
  bf::path src = in_src / key;
  bf::path dst = in_dst / key;
  return common_installer::MoveDir(src, dst);
}

common_installer::Step::Status StepCopyStorageDirectories::precheck() {
  backup_path_ = common_installer::GetBackupPathForPackagePath(
      context_->package_storage->path());

  bs::error_code error_code;
  if (!bf::exists(backup_path_, error_code)) {
    LOG(DEBUG) << "Cannot restore storage directories from: " << backup_path_;
    return Status::ERROR;
  }

  return Status::OK;
}

common_installer::Step::Status StepCopyStorageDirectories::process() {
  if (!MoveAppStorage(backup_path_,
                      context_->package_storage->path(),
                      kDataLocation)) {
    LOG(ERROR) << "Failed to restore private directory for widget in update";
    return Status::ERROR;
  }

  if (!MoveAppStorage(backup_path_,
                      context_->package_storage->path(),
                      kSharedLocation)) {
    LOG(ERROR) << "Failed to restore shared directory for widget in update";
    return Status::ERROR;
  }

  return Status::OK;
}

common_installer::Step::Status StepCopyStorageDirectories::undo() {
  common_installer::Step::Status ret = Status::OK;
  if (!MoveAppStorage(context_->package_storage->path(),
                      backup_path_,
                      kDataLocation)) {
    LOG(ERROR) << "Failed to restore private directory for package in update";
    ret = Status::ERROR;
  }

  if (!MoveAppStorage(context_->package_storage->path(),
                      backup_path_,
                      kSharedLocation)) {
    LOG(ERROR) << "Failed to restore shared directory for package in update";
    ret = Status::ERROR;
  }

  return ret;
}

}  // namespace filesystem
}  // namespace common_installer
