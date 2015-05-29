// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_copy_backup.h"

#include <boost/filesystem/operations.hpp>
#include <boost/system/error_code.hpp>

#include <cassert>
#include <string>

#include "utils/file_util.h"

namespace common_installer {
namespace copy_backup {

namespace bf = boost::filesystem;
namespace bs = boost::system;

Step::Status StepCopyBackup::process() {
  assert(!context_->pkgid.get().empty());

  // set application path
  context_->pkg_path.set(
      context_->root_application_path.get() / context_->pkgid.get());

  install_path_ = context_->pkg_path.get();

  // FIXME: correctly order app's data.
  // If there is 1 app in package, app's data are stored in <pkg_path>/<app_id>
  // If there are >1 apps in package, app's data are stored in <pkg_path>
  // considering that multiple apps data are already separated in folders.
  manifest_x *m = context_->manifest_data.get();
  if ((m->uiapplication && !m->uiapplication->next && !m->serviceapplication) ||
      (m->serviceapplication && !m->serviceapplication->next &&
      !m->uiapplication))
    install_path_ /= bf::path(context_->manifest_data.get()->mainapp_id);

  backup_path_ = context_->pkg_path.get();
  backup_path_ += ".bck";

  // backup old content
  if (!utils::MoveDir(context_->pkg_path.get(), backup_path_)) {
    LOG(ERROR) << "Fail to backup widget directory";
    return Step::Status::ERROR;
  }
  LOG(INFO) << "Old widget context saved to: " << backup_path_;

  // copy new content
  bs::error_code error;
  bf::create_directories(install_path_.parent_path(), error);
  if (error) {
    LOG(ERROR) << "Cannot create widget directory";
    return Status::ERROR;
  }
  if (!utils::MoveDir(context_->unpacked_dir_path.get(), install_path_)) {
    LOG(ERROR) << "Fail to copy tmp dir: " << context_->unpacked_dir_path.get()
               << " to dst dir: " << install_path_;
    return Step::Status::ERROR;
  }

  LOG(INFO) << "Successfully move: " << context_->unpacked_dir_path.get()
            << " to: " << install_path_ << " directory";
  return Status::OK;
}

Step::Status StepCopyBackup::clean() {
  assert(!backup_path_.empty());
  assert(!install_path_.empty());

  if (!CleanBackupDirectory()) {
    LOG(DEBUG) << "Cannot remove backup directory";
    return Status::ERROR;
  }
  LOG(DEBUG) << "Applications files backup directory removed";

  return Status::OK;
}

Step::Status StepCopyBackup::undo() {
  assert(!backup_path_.empty());
  assert(!install_path_.empty());

  // TODO(t.iwanek): this should be done in StepUnzip
  bs::error_code error;
  LOG(DEBUG) << "Remove tmp dir: " << context_->unpacked_dir_path.get();
  bf::remove_all(context_->unpacked_dir_path.get(), error);  // ignore error

  // if backup was created then restore files
  if (bf::exists(backup_path_)) {
    if (!RollbackApplicationDirectory()) {
      LOG(ERROR) << "Failed to revert widget directory";
      return Status::ERROR;
    }
    LOG(DEBUG) << "Application files reverted from backup";
  }
  return Status::OK;
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

  if (!utils::MoveDir(backup_path_, context_->pkg_path.get())) {
    return false;
  }

  return true;
}

}  // namespace copy_backup
}  // namespace common_installer
