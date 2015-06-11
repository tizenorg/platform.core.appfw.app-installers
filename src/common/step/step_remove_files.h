// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_REMOVE_FILES_H_
#define COMMON_STEP_STEP_REMOVE_FILES_H_

#include "common/context_installer.h"

#include "common/step/step.h"
#include "common/utils/logging.h"

namespace common_installer {
namespace remove {

class StepRemoveFiles : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override;
  Status undo() override;
  Status precheck() override;

  SCOPE_LOG_TAG(Remove)
};

}  // namespace remove
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_REMOVE_FILES_H_