// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include <string>

#include "common/utils/subprocess.h"
#include "common/step/step_create_per_user_storage_directories.h"

namespace common_installer {
namespace filesystem {

common_installer::Step::Status StepCreatePerUserStorageDirectories::process() {
  if (GLOBAL_USER != context_->uid.get()) return Step::Status::OK;
  std::string package_id = context_->pkgid.get();
  LOG(INFO) << "Creating per-user directories for package: " << package_id;

  Subprocess pkgdir_tool_process("/usr/bin/pkgdir-tool");
  pkgdir_tool_process.RunWithArgs({"--create", "--pkgid", package_id});
  int result = pkgdir_tool_process.Wait();
  if (result) {
    LOG(ERROR) << "Failed to create shared dirs for users";
    return Status::APP_DIR_ERROR;
  }
  return Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer