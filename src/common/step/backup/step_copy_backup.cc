// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/backup/step_copy_backup.h"

#include <boost/filesystem/operations.hpp>
#include <boost/system/error_code.hpp>

#include <cassert>
#include <string>

#include "common/backup_paths.h"
#include "common/utils/file_util.h"

namespace common_installer {
namespace backup {

namespace bf = boost::filesystem;
namespace bs = boost::system;

Step::Status StepCopyBackup::precheck() {
  if (context_->pkgid.get().empty()) {
    LOG(ERROR) << "pkgid attribute is empty";
    return Step::Status::PACKAGE_NOT_FOUND;
  }
  if (context_->root_application_path.get().empty()) {
    LOG(ERROR) << "root_application_path attribute is empty";
    return Step::Status::INVALID_VALUE;
  }
  return Status::OK;
}

Step::Status StepCopyBackup::process() {
  install_path_ = context_->package_storage->path();
  backup_path_ = GetBackupPathForPackagePath(context_->package_storage->path());

  if (!CreateBackup())
    return Status::APP_DIR_ERROR;

  if (!CreateNewContent())
    return Status::APP_DIR_ERROR;

  return Status::OK;
}

Step::Status StepCopyBackup::clean() {
  if (!CleanBackupDirectory()) {
    LOG(DEBUG) << "Cannot remove backup directory";
    return Status::APP_DIR_ERROR;
  }
  context_->package_storage->Commit();
  return Status::OK;
}

Step::Status StepCopyBackup::undo() {
  // TODO(t.iwanek): this should be done in StepUnzip
  bs::error_code error;
  LOG(DEBUG) << "Remove tmp dir: " << context_->unpacked_dir_path.get();
  bf::remove_all(context_->unpacked_dir_path.get(), error);  // ignore error

  // if backup was created then restore files
  if (bf::exists(backup_path_)) {
    if (!RollbackApplicationDirectory()) {
      LOG(ERROR) << "Failed to revert package directory";
      return Status::APP_DIR_ERROR;
    }
    LOG(DEBUG) << "Application files reverted from backup";
  }
  context_->package_storage->Abort();
  return Status::OK;
}

bool StepCopyBackup::CreateBackup() {
  if (!MoveDir(context_->package_storage->path(), backup_path_)) {
    LOG(ERROR) << "Fail to backup package directory";
    return false;
  }
  LOG(INFO) << "Old package context saved to: " << backup_path_;
  return true;
}

bool StepCopyBackup::CreateNewContent() {
  if (!MoveDir(context_->unpacked_dir_path.get(), install_path_)) {
    LOG(ERROR) << "Fail to copy tmp dir: " << context_->unpacked_dir_path.get()
               << " to dst dir: " << install_path_;
    return false;
  }

  LOG(INFO) << "Successfully move: " << context_->unpacked_dir_path.get()
            << " to: " << install_path_ << " directory";

  return true;
}

bool StepCopyBackup::CleanBackupDirectory() {
  if (bf::exists(backup_path_)) {
    bs::error_code error;
    bf::remove_all(backup_path_, error);
    if (error)
      return false;
  }
  return true;
}

bool StepCopyBackup::RollbackApplicationDirectory() {
  bs::error_code error;
  if (bf::exists(context_->package_storage->path())) {
    bf::remove_all(context_->package_storage->path(), error);
    if (error) {
      return false;
    }
  }

  if (!MoveDir(backup_path_, context_->package_storage->path())) {
    return false;
  }

  return true;
}

}  // namespace backup
}  // namespace common_installer
