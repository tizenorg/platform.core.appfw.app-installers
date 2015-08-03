// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_open_recovery_file.h"

#include <cassert>

#include "common/recovery_file.h"

namespace common_installer {
namespace recovery {

Step::Status StepOpenRecoveryFile::process() {
  std::unique_ptr<recovery::RecoveryFile> recovery_file =
      RecoveryFile::OpenRecoveryFileForPath(context_->file_path.get());
  if (!recovery_file) {
    LOG(ERROR) << "Failed to open recovery file";
    return Status::ERROR;
  }
  context_->unpacked_dir_path.set(recovery_file->unpacked_dir());
  context_->pkgid.set(recovery_file->pkgid());
  switch (recovery_file->type()) {
  case RequestType::Install:
    LOG(INFO) << "Running recovery for new installation";
    break;
  case RequestType::Update:
    LOG(INFO) << "Running recovery for update installation";
    break;
  default:
    assert(false && "Not reached");
  }

  context_->recovery_info.set(RecoveryInfo(std::move(recovery_file)));
  return Status::OK;
}

Step::Status StepOpenRecoveryFile::undo() {
  // Detach object from underlaying file so it will not be removed
  // as recovery failed.
  if (context_->recovery_info.get().recovery_file)
    context_->recovery_info.get().recovery_file->Detach();
  return Status::OK;
}

}  // namespace recovery
}  // namespace common_installer
