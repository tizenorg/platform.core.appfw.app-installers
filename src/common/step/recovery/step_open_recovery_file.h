// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_RECOVERY_STEP_OPEN_RECOVERY_FILE_H_
#define COMMON_STEP_RECOVERY_STEP_OPEN_RECOVERY_FILE_H_

#include <manifest_parser/utils/logging.h>

#include "common/installer_context.h"

#include "common/step/step.h"

namespace common_installer {
namespace recovery {

class __attribute__ ((visibility ("default"))) StepOpenRecoveryFile : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override;
  Status precheck() override { return Status::OK; }

  SCOPE_LOG_TAG(OpenRecoveryFile)
};

}  // namespace recovery
}  // namespace common_installer

#endif  // COMMON_STEP_RECOVERY_STEP_OPEN_RECOVERY_FILE_H_
