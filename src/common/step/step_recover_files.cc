// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_recover_files.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>

#include "common/backup_paths.h"
#include "common/utils/file_util.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace common_installer {
namespace filesystem {

Step::Status StepRecoverFiles::RecoveryNew() {
  if (!SetPackageStorage()) {
    LOG(DEBUG) << "Package files recovery not needed";
    return Status::OK;
  }
  if (bf::exists(context_->package_storage->path())) {
    bs::error_code error;
    bf::remove_all(context_->package_storage->path(), error);
  }
  LOG(INFO) << "Package files recovery done";
  return Status::OK;
}

Step::Status StepRecoverFiles::RecoveryUpdate() {
  if (!SetPackageStorage()) {
    LOG(DEBUG) << "Package files recovery not needed";
    return Status::OK;
  }
  bf::path backup_path = GetBackupPathForPackagePath(
        context_->package_storage->path());
  if (bf::exists(backup_path)) {
    if (bf::exists(context_->package_storage->path())) {
      bs::error_code error;
      bf::remove_all(context_->package_storage->path(), error);
      if (error) {
        LOG(ERROR) << "Cannot restore widget files to its correct location";
        return Status::ERROR;
      }
    }
    (void) MoveDir(backup_path, context_->package_storage->path());
  }
  LOG(INFO) << "Package files recovery done";
  return Status::OK;
}

bool StepRecoverFiles::SetPackageStorage() {
  if (context_->pkgid.get().empty())
    return false;
  // set package storage
  context_->package_storage =
      CreatePackageStorage(RequestType::Recovery,
                           context_->root_application_path.get(),
                           context_->pkgid.get(),
                           context_->unpacked_dir_path.get(),
                           nullptr);
  if (!context_->package_storage) {
    LOG(ERROR) << "Failed to create storage";
    return false;
  }
  return true;
}

}  // namespace filesystem
}  // namespace common_installer

