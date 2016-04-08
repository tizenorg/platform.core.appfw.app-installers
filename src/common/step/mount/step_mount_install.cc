// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "step/mount/step_mount_install.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <string>

#include "common/backup_paths.h"
#include "common/tzip_interface.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace common_installer {
namespace mount {

Step::Status StepMountInstall::process() {
  // set application path
  context_->pkg_path.set(
    context_->root_application_path.get() / context_->pkgid.get());

  bf::path install_path = context_->pkg_path.get();

  bs::error_code error;
  bf::create_directories(install_path.parent_path(), error);
  if (error) {
    LOG(ERROR) << "Cannot create directory: " << install_path.parent_path();
    return Status::APP_DIR_ERROR;
  }

  TzipInterface tzip_unpack(context_->unpacked_dir_path.get().string());
  if (!tzip_unpack.UnMountZip()) {
    LOG(ERROR) << "Failed to unmount zip package from temporary path";
    return Status::APP_DIR_ERROR;
  }

  bf::path mount_point = GetMountLocation(context_->pkg_path.get());
  TzipInterface tzip_final(mount_point.string());
  if (!tzip_final.MountZip(context_->file_path.get().string())) {
    LOG(ERROR) << "Failed to mount zip package in installation path";
    return Status::APP_DIR_ERROR;
  }

  LOG(INFO) << "Successfully mount zip package in: " << mount_point;

  // TODO(t.iwanek): extract bin/, lib/, shared/res/...

  // TODO(t.iwanek): create symlinks for entries...

  return Status::OK;
}

Step::Status StepMountInstall::clean() {
  bf::path mount_point = GetMountLocation(context_->pkg_path.get());
  TzipInterface tzip_final(mount_point.string());
  if (!tzip_final.UnMountZip()) {
    LOG(ERROR) << "Failed to unmount zip package after installation";
    return Status::APP_DIR_ERROR;
  }
  return Status::OK;
}

Step::Status StepMountInstall::undo() {
  bs::error_code error;
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
  if (!boost::filesystem::exists(context_->root_application_path.get())) {
    LOG(ERROR) << "root_application_path ("
               << context_->root_application_path.get()
               << ") path does not exist";
    return Step::Status::INVALID_VALUE;
  }

  if (context_->unpacked_dir_path.get().empty()) {
    LOG(ERROR) << "unpacked_dir_path attribute is empty";
    return Step::Status::INVALID_VALUE;
  }
  if (!boost::filesystem::exists(context_->unpacked_dir_path.get())) {
    LOG(ERROR) << "unpacked_dir_path ("
               << context_->unpacked_dir_path.get()
               << ") path does not exist";
    return Step::Status::INVALID_VALUE;
  }

  if (!context_->manifest_data.get()) {
    LOG(ERROR) << "manifest_data attribute is empty";
    return Step::Status::MANIFEST_NOT_FOUND;
  }

  if (context_->pkgid.get().empty()) {
    LOG(ERROR) << "pkgid attribute is empty";
    return Step::Status::PACKAGE_NOT_FOUND;
  }

  return Step::Status::OK;
}

}  // namespace mount
}  // namespace common_installer
