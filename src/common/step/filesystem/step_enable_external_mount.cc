// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_enable_external_mount.h"

#include "common/external_mount.h"

namespace common_installer {
namespace filesystem {

Step::Status StepEnableExternalMount::process() {
  std::unique_ptr<ExternalMount> mount(new ExternalMount(context_->pkgid.get(),
                                                         context_->uid.get()));
  if (!mount->IsAvailable()) {
    LOG(DEBUG) << "External mount not available for package";
    return Status::OK;
  }

  if (!mount->Mount()) {
    LOG(ERROR) << "Failed to mount package for request";
    return Status::APP_DIR_ERROR;
  }
  context_->external_mount = std::move(mount);
  LOG(DEBUG) << "External mount created";
  return Status::OK;
}

Step::Status StepEnableExternalMount::undo() {
  if (context_->external_mount) {
    if (!context_->external_mount->Umount()) {
      LOG(ERROR) << "Failed to unmount package";
      return Status::APP_DIR_ERROR;
    }
  }
  return Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer

