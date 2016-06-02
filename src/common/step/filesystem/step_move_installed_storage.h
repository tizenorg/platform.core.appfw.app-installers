// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_FILESYSTEM_STEP_MOVE_INSTALLED_STORAGE_H_
#define COMMON_STEP_FILESYSTEM_STEP_MOVE_INSTALLED_STORAGE_H_

#include "common/installer_context.h"
#include "common/step/step.h"

namespace common_installer {
namespace filesystem {

class StepMoveInstalledStorage : public Step {
 public:
  using Step::Step;

  Status process() override;

  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override;

  STEP_NAME(MoveInstalledStorage)

 private:
  app2ext_handle* handle_;
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_FILESYSTEM_STEP_MOVE_INSTALLED_STORAGE_H_
