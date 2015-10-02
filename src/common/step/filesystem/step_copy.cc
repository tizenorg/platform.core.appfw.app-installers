/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_copy.h"

#include <cassert>
#include <cstring>
#include <string>

#include "common/utils/file_util.h"

namespace common_installer {
namespace filesystem {

namespace bf = boost::filesystem;
namespace bs = boost::system;

Step::Status StepCopy::precheck() {
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

Step::Status StepCopy::process() {
  bf::path install_path = context_->package_storage->path();
  bs::error_code error;
  bf::create_directories(install_path.parent_path(), error);
  if (error) {
    LOG(ERROR) << "Cannot create directory: "
               << install_path.parent_path().string();
    return Step::Status::APP_DIR_ERROR;
  }

  if (!MoveDir(context_->unpacked_dir_path.get(), install_path,
               FSFlag::FS_MERGE_DIRECTORIES)) {
    LOG(ERROR) << "Cannot move widget directory to install path, from "
        << context_->unpacked_dir_path.get() << " to " << install_path;
    return Status::APP_DIR_ERROR;
  }

  LOG(INFO) << "Successfully move: " << context_->unpacked_dir_path.get()
            << " to: " << install_path << " directory";
  return Status::OK;
}

Step::Status StepCopy::undo() {
  bs::error_code error;
  bf::remove_all(context_->package_storage->path(), error);
  context_->package_storage->Abort();
  return error ? Status::ERROR : Status::OK;
}

Step::Status StepCopy::clean() {
  context_->package_storage->Commit();
  return Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer
