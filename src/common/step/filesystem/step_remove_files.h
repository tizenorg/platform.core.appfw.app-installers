// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_FILESYSTEM_STEP_REMOVE_FILES_H_
#define COMMON_STEP_FILESYSTEM_STEP_REMOVE_FILES_H_

#include <manifest_parser/utils/logging.h>

#include "common/installer_context.h"

#include "common/step/step.h"

namespace common_installer {
namespace filesystem {

class StepRemoveFiles : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override  { return Status::OK; }
  Status precheck() override;

  STEP_NAME(Remove)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_FILESYSTEM_STEP_REMOVE_FILES_H_
