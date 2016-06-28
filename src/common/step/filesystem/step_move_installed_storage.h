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

  Status clean() override;
  Status undo() override;
  Status precheck() override;

 private:
  void SetTepPaths();
  bool MoveTep();
  bool MoveBackTep();

  boost::filesystem::path old_location_;
  boost::filesystem::path new_location_;

  STEP_NAME(MoveInstalledStorage)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_FILESYSTEM_STEP_MOVE_INSTALLED_STORAGE_H_
