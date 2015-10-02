// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_remove_files.h"

#include <boost/system/error_code.hpp>

#include "common/backup_paths.h"
#include "common/utils/file_util.h"

namespace common_installer {
namespace filesystem {

namespace bs = boost::system;
namespace bf = boost::filesystem;

Step::Status StepRemoveFiles::precheck() {
  if (!context_->manifest_data.get()) {
    LOG(ERROR) << "manifest_data attribute is empty";
    return Step::Status::INVALID_VALUE;
  }

  // Even though, the below checks can fail, StepRemoveFiles should still try
  // to remove the files
  if (context_->package_storage->path().empty())
    LOG(ERROR) << "pkg_path attribute is empty";
  else if (!bf::exists(context_->package_storage->path()))
    LOG(ERROR) << "pkg_path ("
               << context_->package_storage->path()
               << ") path does not exist";
  // TODO(p.sikorski) check context_->uid.get()

  return Step::Status::OK;
}

Step::Status StepRemoveFiles::process() {
  bf::path backup_path = GetBackupPathForPackagePath(
      context_->package_storage->path());
  if (!MoveDir(context_->package_storage->path(), backup_path)) {
    LOG(ERROR) << "Cannot remove widget files from its location";
    return Status::ERROR;
  }
  LOG(DEBUG) << "Removed directory: " << context_->package_storage->path();
  return Status::OK;
}

Step::Status StepRemoveFiles::clean() {
  bs::error_code error;
  bf::path backup_path = GetBackupPathForPackagePath(
      context_->package_storage->path());
  bf::remove_all(backup_path, error);
  return Status::OK;
}

Step::Status StepRemoveFiles::undo() {
  bf::path backup_path = GetBackupPathForPackagePath(
      context_->package_storage->path());
  if (bf::exists(backup_path)) {
    LOG(DEBUG) << "Restoring directory: " << context_->package_storage->path();
    if (!MoveDir(backup_path, context_->package_storage->path())) {
      LOG(ERROR) << "Cannot restore widget files";
      return Status::ERROR;
    }
  }

  return Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer
