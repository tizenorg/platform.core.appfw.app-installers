// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_wgt_copy_storage_directories.h"

#include <boost/system/error_code.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include "common/utils/file_util.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace {

const char kDataLocation[] = "data";
const char kSharedLocation[] = "shared";
const char kSharedDataLocation[] = "shared/data";
const char kSharedTrustedLocation[] = "shared/trusted";
const char kResWgtSubPath[] = "res/wgt";
const char kTemporaryData[] = "tmp";
const char kCacheDir[] = "cache";

}  // namespace

namespace wgt {
namespace filesystem {

common_installer::Step::Status StepWgtCopyStorageDirectories::process() {
  Status status = CreatePrivateTmpDir();
  if (status != Status::OK)
    return status;
  status = CreateCacheDir();
  if (status != Status::OK)
    return status;

  int version = context_->manifest_data.get()->api_version[0] - '0';
  if (version < 3) {
    LOG(DEBUG) << "Shared directory coping for tizen 2.x";
    return StepCopyStorageDirectories::process();
  }

  LOG(DEBUG) << "Shared directory coping for tizen 3.x";
  status = CopySharedDirectory();
  if (status != Status::OK)
    return status;

  return CopyDataDirectory();
}

common_installer::Step::Status StepWgtCopyStorageDirectories::undo() {
  int version = context_->manifest_data.get()->api_version[0] - '0';
  if (version < 3) {
    LOG(DEBUG) << "Shared directory coping for tizen 2.x";
    return StepCopyStorageDirectories::undo();
  }

  UndoSharedDirectory();
  UndoDataDirectory();
  return Status::OK;
}

void StepWgtCopyStorageDirectories::UndoSharedDirectory() {
  if (!MoveAppStorage(context_->pkg_path.get(),
                      backup_path_,
                      kSharedDataLocation)) {
    LOG(ERROR) <<
        "Failed to undo moving of shared/data directory for widget in update";
  }

  if (!MoveAppStorage(context_->pkg_path.get(),
                      backup_path_,
                      kSharedTrustedLocation)) {
    LOG(ERROR) << "Failed to undo moving of shared/trusted directory"
               << "for widget in update";
  }
}

void StepWgtCopyStorageDirectories::UndoDataDirectory() {
  if (!MoveAppStorage(context_->pkg_path.get(),
                      backup_path_,
                      kDataLocation)) {
    LOG(ERROR)
        << "Failed to undo moving of private directory for widget in update";
  }
}

common_installer::Step::Status
StepWgtCopyStorageDirectories::HandleWgtSharedDirectory() {
  bf::path res_wgt_path = context_->pkg_path.get() / kResWgtSubPath;
  bf::path shared_source = res_wgt_path / kSharedLocation;
  bf::path shared_destination = context_->pkg_path.get() / kSharedLocation;

  // Move shared if exist in wgt
  if (bf::exists(shared_source)) {
    if (!common_installer::MoveDir(shared_source, shared_destination)) {
      LOG(ERROR) << "Failed to move shared data from res/wgt to shared";
      return Status::ERROR;
    }
  }

  // Create shared directory if not present yet
  if (!bf::exists(shared_destination)) {
    bs::error_code error_code;
    bf::create_directory(shared_destination, error_code);
    if (error_code) {
      LOG(ERROR) << "Failed to create shared storage directory";
      return Status::ERROR;
    }
  }

  // Symlink created shared directory
  bs::error_code error_code;
  bf::create_symlink(shared_destination, shared_source, error_code);
  if (error_code) {
    LOG(ERROR) << "Failed to create symbolic link for shared dir"
               << boost::system::system_error(error_code).what();
    return Status::ERROR;
  }
  return Status::OK;
}

common_installer::Step::Status
StepWgtCopyStorageDirectories::CopySharedDirectory() {
  Status status = HandleWgtSharedDirectory();
  if (status != Status::OK)
    return status;

  if (!MoveAppStorage(backup_path_,
                      context_->pkg_path.get(),
                      kSharedDataLocation)) {
    LOG(ERROR) <<
        "Failed to restore shared/data directory for widget in update";
    return Status::ERROR;
  }

  if (!MoveAppStorage(backup_path_,
                      context_->pkg_path.get(),
                      kSharedTrustedLocation)) {
    LOG(ERROR) <<
        "Failed to restore shared/trusted directory for widget in update";
    return Status::ERROR;
  }

  return Status::OK;
}

common_installer::Step::Status
StepWgtCopyStorageDirectories::CopyDataDirectory() {
  if (!MoveAppStorage(backup_path_,
                      context_->pkg_path.get(),
                      kDataLocation)) {
    LOG(ERROR) << "Failed to restore private directory for widget in update";
    return Status::ERROR;
  }
  return Status::OK;
}

common_installer::Step::Status
StepWgtCopyStorageDirectories::CreatePrivateTmpDir() {
  bs::error_code error_code;
  bf::path tmp_path = context_->pkg_path.get() / kTemporaryData;
  bf::create_directory(tmp_path, error_code);
  if (error_code) {
    LOG(ERROR) << "Failed to create private temporary directory for package";
    return Status::ERROR;
  }
  return Status::OK;
}

common_installer::Step::Status
StepWgtCopyStorageDirectories::CreateCacheDir() {
  bs::error_code error_code;
  bf::path cache_path = context_->pkg_path.get() / kCacheDir;
  bf::create_directory(cache_path, error_code);
  if (error_code) {
    LOG(ERROR) << "Failed to create cache directory for package";
    return Status::ERROR;
  }
  return Status::OK;
}

}  // namespace filesystem
}  // namespace wgt

