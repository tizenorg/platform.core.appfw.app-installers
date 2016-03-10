// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.
#include "common/step/filesystem/step_remove_temporary_directory.h"

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

#include "common/installer_context.h"

namespace common_installer {
namespace filesystem {

namespace bf = boost::filesystem;

Step::Status StepRemoveTemporaryDirectory::RecoveryUpdate() {
  RemoveFiles();
  return Step::Status::OK;
}

Step::Status StepRemoveTemporaryDirectory::RecoveryNew() {
  RemoveFiles();
  return Step::Status::OK;
}

void StepRemoveTemporaryDirectory::RemoveFiles() {
  bf::path unpack_dir_path = context_->unpacked_dir_path.get();
  if (unpack_dir_path.empty())
    return;
  boost::system::error_code error_code;
  bf::remove_all(unpack_dir_path, error_code);
  return;
}
}  // namespace filesystem
}  // namespace common_installer

