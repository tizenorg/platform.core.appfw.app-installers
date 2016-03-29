// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_RECOVERY_STEP_RECOVERY_H_
#define COMMON_STEP_RECOVERY_STEP_RECOVERY_H_

#include "common/installer_context.h"

#include "common/step/step.h"

namespace common_installer {
namespace recovery {

/**
 * @brief The StepRecovery class
 *
 * Common base of all steps that performs recovery. Recover is supported for
 * new and update request modes. Each step that performs recovery needs
 * to override methods:
 *  - RecoveryNew(),
 *  - RecoveryUpdate(),
 * instead of:
 *  - process().
 */
class StepRecovery : public Step {
 public:
  using Step::Step;

  __attribute__ ((visibility ("default"))) Status process() override;
  __attribute__ ((visibility ("default"))) Status clean() override { return Status::OK; }
  __attribute__ ((visibility ("default"))) Status undo() override { return Status::OK; }
  __attribute__ ((visibility ("default"))) Status precheck() override { return Status::OK; }

  __attribute__ ((visibility ("default"))) virtual Status RecoveryNew() = 0;
  __attribute__ ((visibility ("default"))) virtual Status RecoveryUpdate() = 0;
};

}  // namespace recovery
}  // namespace common_installer

#endif  // COMMON_STEP_RECOVERY_STEP_RECOVERY_H_
