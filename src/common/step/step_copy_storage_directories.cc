// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_copy_storage_directories.h"

#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>

#include <string>

#include "utils/file_util.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace {

const char kDataLocation[] = "data";
const char kSharedLocation[] = "shared";

bool RestoreApplicationStorageForData(const bf::path& pkg_path) {
  bf::path backup_path =
      common_installer::GetBackupPathForPackagePath(pkg_path);

  bs::error_code error_code;
  if (!bf::exists(backup_path, error_code)) {
    LOG(DEBUG) << "Cannot restore storage directories from: " << backup_path;
    return false;
  }

  bf::path data_path = pkg_path / kDataLocation;
  bf::path data_backup_path = backup_path / kDataLocation;
  if (!common_installer::utils::CopyDir(data_backup_path, data_path)) {
    LOG(ERROR) << "Failed to restore private directory for widget in update";
    return false;
  }

  bf::path shared_path = pkg_path / kSharedLocation;
  bf::path shared_backup_path = backup_path / kSharedLocation;
  if (!common_installer::utils::CopyDir(shared_backup_path, shared_path)) {
    LOG(ERROR) << "Failed to restore shared directory for widget in update";
    return false;
  }

  return true;
}

}  // namespace

namespace common_installer {
namespace copy_storage {

common_installer::Step::Status StepCopyStorageDirectories::process() {
  if (!RestoreApplicationStorageForData(context_->pkg_path.get()))
    return Status::ERROR;
  return Status::OK;
}

}  // namespace copy_storage
}  // namespace common_installer
