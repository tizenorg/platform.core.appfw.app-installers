// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_ROLLBACK_INSTALLATION_SECURITY_H_
#define COMMON_STEP_STEP_ROLLBACK_INSTALLATION_SECURITY_H_

#include "common/step/step.h"
#include "common/utils/logging.h"

namespace common_installer {
namespace security {

// Step that is used during installation to rollback security changes if
// installation fails. Those changes cannot be reverted in StepSecurity
// because its to early. We need to remove package files first.
class StepRollbackInstallationSecurity : public Step {
 public:
  using Step::Step;

  Status process() override { return Status::OK; }
  Status undo() override;
  Status clean() override { return Status::OK; }
  Status precheck() override;

  SCOPE_LOG_TAG(RollbackInstallationSecurity)
};

}  // namespace security
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_ROLLBACK_INSTALLATION_SECURITY_H_
