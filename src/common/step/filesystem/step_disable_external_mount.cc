// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_disable_external_mount.h"

namespace common_installer {
namespace filesystem {

Step::Status StepDisableExternalMount::process() {
  if (context_->external_mount) {
    if (!context_->external_mount->Umount()) {
      LOG(ERROR) << "Failed to unmount package for request";
      return Status::APP_DIR_ERROR;
    }
  }
  context_->external_mount.reset();
  LOG(DEBUG) << "External mount removed";
  return Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer
