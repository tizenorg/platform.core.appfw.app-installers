// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_copy_storage_directories.h"

#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>

#include <string>

#include "common/paths.h"
#include "utils/file_util.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace {

const char kCache[] = "cache";
const char kDataLocation[] = "data";
const char kSharedResLocation[] = "shared";

}  // namespace

namespace common_installer {
namespace filesystem {

bool StepCopyStorageDirectories::MoveAppStorage(
    const bf::path& in_src,
    const bf::path& in_dst,
    const char *key,
    bool merge_dirs) {
  bf::path src = in_src / key;
  bf::path dst = in_dst / key;
  return common_installer::MoveDir(src, dst,
      merge_dirs ? common_installer::FS_MERGE_DIRECTORIES
                 : common_installer::FS_NONE);
}

common_installer::Step::Status StepCopyStorageDirectories::precheck() {
  backup_path_ =
      common_installer::GetBackupPathForPackagePath(context_->pkg_path.get());

  bs::error_code error_code;
  if (!bf::exists(backup_path_, error_code)) {
    LOG(DEBUG) << "Cannot restore storage directories from: " << backup_path_;
    return Status::INVALID_VALUE;
  }

  return Status::OK;
}

common_installer::Step::Status StepCopyStorageDirectories::process() {
  if (context_->request_mode.get() == RequestMode::GLOBAL)
    return Status::OK;
  if (!MoveAppStorage(backup_path_,
                      context_->pkg_path.get(),
                      kDataLocation, true)) {
    LOG(ERROR) << "Failed to restore private directory for widget in update";
    return Status::APP_DIR_ERROR;
  }

  if (!MoveAppStorage(backup_path_,
                      context_->pkg_path.get(),
                      kSharedResLocation, true)) {
    LOG(ERROR) << "Failed to restore shared directory for widget in update";
    return Status::APP_DIR_ERROR;
  }

  if (!CacheDir())
    return Status::APP_DIR_ERROR;

  return Status::OK;
}

common_installer::Step::Status StepCopyStorageDirectories::undo() {
  common_installer::Step::Status ret = Status::OK;
  if (!MoveAppStorage(context_->pkg_path.get(),
                      backup_path_,
                      kDataLocation)) {
    LOG(ERROR) << "Failed to restore private directory for package in update";
    ret = Status::APP_DIR_ERROR;
  }

  if (!MoveAppStorage(context_->pkg_path.get(),
                      backup_path_,
                      kSharedResLocation)) {
    LOG(ERROR) << "Failed to restore shared directory for package in update";
    ret = Status::APP_DIR_ERROR;
  }

  return ret;
}

bool StepCopyStorageDirectories::CacheDir() {
  bs::error_code error_code;
  bf::path cache_path = context_->pkg_path.get() / kCache;
  bf::create_directory(cache_path, error_code);
  if (error_code) {
    LOG(ERROR) << "Failed to create cache directory for package";
    return false;
  }
  return true;
}

}  // namespace filesystem
}  // namespace common_installer
