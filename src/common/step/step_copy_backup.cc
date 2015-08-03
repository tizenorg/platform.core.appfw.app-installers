// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_copy_backup.h"

#include <boost/filesystem/operations.hpp>
#include <boost/system/error_code.hpp>

#include <cassert>
#include <string>

#include "common/utils/file_util.h"

namespace common_installer {
namespace backup {

namespace bf = boost::filesystem;
namespace bs = boost::system;

Step::Status StepCopyBackup::precheck() {
  if (context_->pkgid.get().empty()) {
    LOG(ERROR) << "pkgid attribute is empty";
    return Step::Status::INVALID_VALUE;
  }
  if (context_->root_application_path.get().empty()) {
    LOG(ERROR) << "root_application_path attribute is empty";
    return Step::Status::INVALID_VALUE;
  }

  // set package path
  context_->pkg_path.set(
      context_->root_application_path.get() / context_->pkgid.get());
  install_path_ = context_->pkg_path.get();
  backup_path_ = GetBackupPathForPackagePath(context_->pkg_path.get());

  return Status::OK;
}

Step::Status StepCopyBackup::process() {
  if (!Backup())
    return Status::ERROR;

  if (!NewContent())
    return Status::ERROR;

  return Status::OK;
}

Step::Status StepCopyBackup::clean() {
  if (!CleanBackupDirectory()) {
    LOG(DEBUG) << "Cannot remove backup directory";
    return Status::ERROR;
  }
  LOG(DEBUG) << "Applications files backup directory removed";

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
      return Status::ERROR;
    }
    LOG(DEBUG) << "Application files reverted from backup";
  }
  return Status::OK;
}

bool StepCopyBackup::Backup() {
  if (!MoveDir(context_->pkg_path.get(), backup_path_)) {
    LOG(ERROR) << "Fail to backup package directory";
    return false;
  }
  LOG(INFO) << "Old package context saved to: " << backup_path_;
  return true;
}

bool StepCopyBackup::NewContent() {
  bs::error_code error;
  bf::create_directories(install_path_.parent_path(), error);
  if (error) {
    LOG(ERROR) << "Cannot create package directory";
    return false;
  }
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
  if (bf::exists(context_->pkg_path.get())) {
    bf::remove_all(context_->pkg_path.get(), error);
    if (error) {
      return false;
    }
  }

  if (!MoveDir(backup_path_, context_->pkg_path.get())) {
    return false;
  }

  return true;
}

}  // namespace backup
}  // namespace common_installer
