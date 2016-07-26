// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/backup/step_copy_backup.h"

#include <boost/filesystem/operations.hpp>
#include <boost/system/error_code.hpp>

#include <cassert>
#include <string>

#include "common/paths.h"
#include "common/utils/file_util.h"

namespace {

const char kExternalMemoryMountPoint[] = ".mmc";

}

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

  // set package path
  context_->pkg_path.set(
      context_->root_application_path.get() / context_->pkgid.get());
  install_path_ = context_->pkg_path.get();
  backup_path_ = GetBackupPathForPackagePath(context_->pkg_path.get());

  return Status::OK;
}

Step::Status StepCopyBackup::process() {
  if (!Backup())
    return Status::APP_DIR_ERROR;

  if (!NewContent())
    return Status::APP_DIR_ERROR;

  return Status::OK;
}

Step::Status StepCopyBackup::clean() {
  if (!CleanBackupDirectory()) {
    LOG(DEBUG) << "Cannot remove backup directory";
    return Status::APP_DIR_ERROR;
  }
  LOG(DEBUG) << "Applications files backup directory removed";

  if (context_->external_storage)
    context_->external_storage->Commit();

  return Status::OK;
}

Step::Status StepCopyBackup::undo() {
  // TODO(t.iwanek): this should be done in StepUnzip
  bs::error_code error;
  LOG(DEBUG) << "Remove tmp dir: " << context_->unpacked_dir_path.get();
  bf::remove_all(context_->unpacked_dir_path.get(), error);  // ignore error

  if (context_->external_storage)
    context_->external_storage->Abort();

  // if backup was created then restore files
  if (bf::exists(backup_path_)) {
    if (!RollbackApplicationDirectory()) {
      LOG(ERROR) << "Failed to revert package directory";
      return Status::APP_DIR_ERROR;
    }
    LOG(DEBUG) << "Application files reverted from backup";
  }
  return Status::OK;
}

bool StepCopyBackup::Backup() {
  // create backup directory
  bs::error_code error;
  bf::create_directories(backup_path_, error);

  // create copy of old package content skipping the external memory mount point
  for (bf::directory_iterator iter(context_->pkg_path.get());
       iter != bf::directory_iterator(); ++iter) {
    if (iter->path().filename() == kExternalMemoryMountPoint)
      continue;

    // external storage directories are mounted
    // therefore move only content
    if (context_->external_storage) {
      auto& ext_dirs = context_->external_storage->external_dirs();
      auto found = std::find(ext_dirs.begin(), ext_dirs.end(),
                             iter->path().filename());
      if (found != ext_dirs.end()) {
        bool done = MoveMountPointContent(iter->path(),
            backup_path_ / iter->path().filename());
        if (!done) {
          LOG(ERROR) << "Failed to move: " << iter->path();
          return false;
        }
        continue;
      }
    }

    if (bf::is_directory(iter->path())) {
      if (!MoveDir(iter->path(), backup_path_ / iter->path().filename())) {
        LOG(ERROR) << "Fail to backup package directory of: " << iter->path();
        return false;
      }
    } else {
      if (!MoveFile(iter->path(), backup_path_ / iter->path().filename())) {
        LOG(ERROR) << "Fail to backup package file: " << iter->path();
        return false;
      }
    }
  }
  LOG(INFO) << "Old package context saved to: " << backup_path_;
  return true;
}


bool StepCopyBackup::MoveMountPointContent(const boost::filesystem::path& from,
                                           const boost::filesystem::path& to) {
  bs::error_code error;
  bf::create_directories(to, error);

  for (bf::directory_iterator iter(from);
       iter != bf::directory_iterator(); ++iter) {
    if (bf::is_directory(iter->path())) {
      if (!MoveDir(iter->path(), to / iter->path().filename())) {
        LOG(ERROR) << "Fail to backup package directory of: " << iter->path();
        return false;
      }
    } else if (bf::is_symlink(symlink_status(iter->path()))) {
      bs::error_code error;
      bf::copy_symlink(iter->path(), to / iter->path().filename(), error);
      if (error) {
        LOG(ERROR) << "Failed to backup package symlink: " << iter->path();
        return false;
      }
    } else {
      if (!MoveFile(iter->path(), to / iter->path().filename())) {
        LOG(ERROR) << "Fail to backup package file: " << iter->path();
        return false;
      }
    }
  }
  return true;
}

bool StepCopyBackup::NewContent() {
  bs::error_code error;
  bf::create_directories(install_path_.parent_path(), error);
  if (error) {
    LOG(ERROR) << "Cannot create package directory";
    return false;
  }
  if (!MoveDir(context_->unpacked_dir_path.get(), install_path_,
               FSFlag::FS_MERGE_DIRECTORIES)) {
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
