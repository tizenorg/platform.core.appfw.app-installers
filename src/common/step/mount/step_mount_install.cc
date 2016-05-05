// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "step/mount/step_mount_install.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <string>

#include "common/backup_paths.h"
#include "common/request.h"
#include "common/tzip_interface.h"
#include "common/utils/file_util.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace common_installer {
namespace mount {

Step::Status StepMountInstall::process() {
  context_->pkg_path.set(
      context_->root_application_path.get() / context_->pkgid.get());

  TzipInterface tzip_unpack(context_->unpacked_dir_path.get());
  if (!tzip_unpack.UnmountZip()) {
    LOG(ERROR) << "Failed to unmount zip package from temporary path";
    return Status::APP_DIR_ERROR;
  }

  bf::path zip_destination_path =
      GetZipPackageLocation(context_->pkg_path.get(), context_->pkgid.get());
  if (!bf::exists(zip_destination_path.parent_path())) {
    bs::error_code error;
    bf::create_directories(zip_destination_path.parent_path(), error);
    if (error) {
      LOG(ERROR) << "Failed to create diretory: "
                 << zip_destination_path.parent_path();
      return Status::APP_DIR_ERROR;
    }
  }

  if (!CopyFile(context_->file_path.get(), zip_destination_path)) {
    return Status::APP_DIR_ERROR;
  }
  context_->manifest_data.get()->mount_point =
      strdup(zip_destination_path.c_str());

  bf::path mount_point = GetMountLocation(context_->pkg_path.get());
  if (!bf::exists(mount_point)) {
    bs::error_code error;
    bf::create_directories(mount_point, error);
    if (error) {
      LOG(ERROR) << "Failed to create mount point directory: " << mount_point;
      return Status::APP_DIR_ERROR;
    }
  }
  TzipInterface tzip_final(mount_point);
  if (!tzip_final.MountZip(zip_destination_path)) {
    LOG(ERROR) << "Failed to mount zip package in installation path";
    return Status::APP_DIR_ERROR;
  }

  LOG(INFO) << "Successfully mount zip package in: " << mount_point;
  return Status::OK;
}

Step::Status StepMountInstall::clean() {
  bf::path mount_point = GetMountLocation(context_->pkg_path.get());
  TzipInterface tzip_final(mount_point);
  if (!tzip_final.UnmountZip()) {
    LOG(ERROR) << "Failed to unmount zip package after installation";
    return Status::APP_DIR_ERROR;
  }
  bs::error_code error;
  bf::remove(mount_point, error);
  return Status::OK;
}

Step::Status StepMountInstall::undo() {
  bs::error_code error;
  bf::path mount_point = GetMountLocation(context_->pkg_path.get());
  TzipInterface tzip_final(mount_point);
  tzip_final.UnmountZip();
  bf::remove(GetZipPackageLocation(context_->pkg_path.get(),
                                   context_->pkgid.get()), error);
  bf::remove(context_->pkg_path.get(), error);
  if (error) {
    LOG(ERROR) << "Failed to remove package content";
    return Status::APP_DIR_ERROR;
  }
  return Status::OK;
}

Step::Status StepMountInstall::precheck() {
  if (context_->root_application_path.get().empty()) {
    LOG(ERROR) << "root_application_path attribute is empty";
    return Step::Status::INVALID_VALUE;
  }
  if (!bf::exists(context_->root_application_path.get())) {
    LOG(ERROR) << "root_application_path ("
               << context_->root_application_path.get()
               << ") path does not exist";
    return Step::Status::INVALID_VALUE;
  }

  if (context_->unpacked_dir_path.get().empty()) {
    LOG(ERROR) << "unpacked_dir_path attribute is empty";
    return Step::Status::INVALID_VALUE;
  }
  if (!bf::exists(context_->unpacked_dir_path.get())) {
    LOG(ERROR) << "unpacked_dir_path ("
               << context_->unpacked_dir_path.get()
               << ") path does not exist";
    return Step::Status::INVALID_VALUE;
  }

  if (context_->pkgid.get().empty()) {
    LOG(ERROR) << "pkgid attribute is empty";
    return Step::Status::PACKAGE_NOT_FOUND;
  }

  return Step::Status::OK;
}

}  // namespace mount
}  // namespace common_installer
