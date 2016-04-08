// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/mount/step_mount_unpacked.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <unistd.h>

#include <string>

#include "common/utils/file_util.h"
#include "common/tzip_interface.h"

namespace bf = boost::filesystem;

namespace common_installer {
namespace mount {

Step::Status StepMountUnpacked::process() {
  bf::path tmp_dir = GenerateTmpDir(context_->root_application_path.get());

  // TODO(t.iwanek): fix write unpacked directory to recovery file...
  // consider how this will affect recovery mode

  if (!CreateDir(tmp_dir)) {
    LOG(ERROR) << "Failed to create temp directory: " << tmp_dir;
    return Step::Status::APP_DIR_ERROR;
  }
  context_->unpacked_dir_path.set(tmp_dir);

  TzipInterface tzip(context_->unpacked_dir_path.get().string());
  if (!tzip.MountZip(context_->file_path.get().string())) {
    LOG(ERROR) << "Failed to mount zip file: " << context_->file_path.get();
    return Status::IMAGE_ERROR;
  }
  return Status::OK;
}

Step::Status StepMountUnpacked::undo() {
  TzipInterface tzip(context_->unpacked_dir_path.get().string());
  if (!tzip.UnmountZip()) {
    LOG(ERROR) << "Failed to unmount zip file: " << context_->file_path.get();
    return Status::IMAGE_ERROR;
  }

  if (access(context_->unpacked_dir_path.get().string().c_str(), F_OK) == 0) {
    bf::remove_all(context_->unpacked_dir_path.get());
    LOG(DEBUG) << "remove temp dir: " << context_->unpacked_dir_path.get();
  }
  return Status::OK;
}

Step::Status StepMountUnpacked::precheck() {
  if (context_->file_path.get().empty()) {
    LOG(ERROR) << "file_path attribute is empty";
    return Step::Status::INVALID_VALUE;
  }
  if (!bf::exists(context_->file_path.get())) {
    LOG(ERROR) << "file_path ("
               << context_->file_path.get()
               << ") path does not exist";
    return Step::Status::INVALID_VALUE;
  }
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
  return Status::OK;
}

}  // namespace mount
}  // namespace common_installer
