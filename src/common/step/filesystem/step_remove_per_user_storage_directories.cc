// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_remove_per_user_storage_directories.h"

#include <string>
#include <vector>

#include "common/installer_context.h"
#include "common/pkgdir_tool_request.h"
#include "common/shared_dirs.h"

namespace common_installer {
namespace filesystem {

Step::Status StepRemovePerUserStorageDirectories::process() {
  if (context_->request_mode.get() != RequestMode::GLOBAL)
    return Step::Status::OK;

  std::string package_id = context_->pkgid.get();
  if (!common_installer::DeleteSkelDirectories(package_id)) {
    LOG(ERROR) << "Failed to delete skel dirs";
    return Status::APP_DIR_ERROR;
  }

  if (!common_installer::RequestDeleteUserDirectories(package_id)) {
    LOG(ERROR) << "Failed to delete shared dirs for users";
    return Status::APP_DIR_ERROR;
  }

  return Step::Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer

