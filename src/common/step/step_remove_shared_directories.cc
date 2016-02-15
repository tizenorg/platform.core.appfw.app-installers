// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_remove_shared_directories.h"

#include <string>
#include <vector>

#include "common/installer_context.h"
#include "common/pkgmgr_registration.h"
#include "common/utils/subprocess.h"

namespace common_installer {
namespace filesystem {

Step::Status StepRemoveSharedDirectories::process() {
  std::string package_id = context_->pkgid.get();
  bool is_global_installed = IsPackageInstalled(package_id, GLOBAL_USER);
  bool is_local_installed = IsPackageInstalled(package_id, getuid());

  if (context_->request_mode.get() == RequestMode::GLOBAL
      && is_local_installed) {
    LOG(INFO) << "Skipping removal of shared directories because "
        << "local app is installed";
    return Step::Status::OK;
  }

  if (context_->request_mode.get() == RequestMode::USER
      && is_global_installed) {
    LOG(INFO) << "Skipping removal of shared directories because "
        << "global app is installed";
    return Step::Status::OK;
  }

  Subprocess pkgdir_tool_process("/usr/bin/pkgdir-tool");
  pkgdir_tool_process.RunWithArgs({"--delete", "--pkgid", package_id});
  int result = pkgdir_tool_process.Wait();
  if (result) {
    LOG(ERROR) << "Failed to delete shared dirs for users";
    return Step::Status::APP_DIR_ERROR;
  }
  return Step::Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer

