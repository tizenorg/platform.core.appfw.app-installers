// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_recover_files.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>

#include "common/utils/file_util.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace common_installer {
namespace filesystem {

Step::Status StepRecoverFiles::RecoveryNew() {
  if (!SetPackagePath()) {
    LOG(DEBUG) << "Package files recovery not needed";
    return Status::OK;
  }
  if (bf::exists(context_->pkg_path.get())) {
    bs::error_code error;
    bf::remove_all(context_->pkg_path.get(), error);
  }
  LOG(INFO) << "Package files recovery done";
  return Status::OK;
}

Step::Status StepRecoverFiles::RecoveryUpdate() {
  if (!SetPackagePath()) {
    LOG(DEBUG) << "Package files recovery not needed";
    return Status::OK;
  }
  bf::path backup_path = GetBackupPathForPackagePath(context_->pkg_path.get());
  if (bf::exists(backup_path)) {
    if (bf::exists(context_->pkg_path.get())) {
      bs::error_code error;
      bf::remove_all(context_->pkg_path.get(), error);
      if (error) {
        LOG(ERROR) << "Cannot restore widget files to its correct location";
        return Status::ERROR;
      }
    }
    (void) MoveDir(backup_path, context_->pkg_path.get());
  }
  LOG(INFO) << "Package files recovery done";
  return Status::OK;
}

bool StepRecoverFiles::SetPackagePath() {
  if (context_->pkgid.get().empty())
    return false;
  context_->pkg_path.set(
      context_->root_application_path.get() / context_->pkgid.get());
  return true;
}

}  // namespace filesystem
}  // namespace common_installer

