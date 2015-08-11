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
namespace fs = boost::filesystem;

Step::Status StepRemoveFiles::precheck() {
  if (!context_->manifest_data.get()) {
    LOG(ERROR) << "manifest_data attribute is empty";
    return Step::Status::INVALID_VALUE;
  }

  // Even though, the below checks can fail, StepRemoveFiles should still try
  // to remove the files
  if (context_->pkg_path.get().empty())
    LOG(ERROR) << "pkg_path attribute is empty";
  else if (!fs::exists(context_->pkg_path.get()))
    LOG(ERROR) << "pkg_path ("
               << context_->pkg_path.get()
               << ") path does not exist";
  // TODO(p.sikorski) check context_->uid.get()

  return Step::Status::OK;
}

Step::Status StepRemoveFiles::process() {
  fs::path backup_path = GetBackupPathForPackagePath(context_->pkg_path.get());
  if (!MoveDir(context_->pkg_path.get(), backup_path)) {
    LOG(ERROR) << "Cannot remove widget files from its location";
    return Status::ERROR;
  }
  LOG(DEBUG) << "Removed directory: " << context_->pkg_path.get();
  return Status::OK;
}

Step::Status StepRemoveFiles::clean() {
  bs::error_code error;
  fs::path backup_path = GetBackupPathForPackagePath(context_->pkg_path.get());
  fs::remove_all(backup_path, error);
  return Status::OK;
}

Step::Status StepRemoveFiles::undo() {
  fs::path backup_path = GetBackupPathForPackagePath(context_->pkg_path.get());
  if (fs::exists(backup_path)) {
    LOG(DEBUG) << "Restoring directory: " << context_->pkg_path.get();
    if (!MoveDir(backup_path, context_->pkg_path.get())) {
      LOG(ERROR) << "Cannot restore widget files";
      return Status::ERROR;
    }
  }

  return Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer
