// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_SECURITY_STEP_ROLLBACK_DEINSTALLATION_SECURITY_H_
#define COMMON_STEP_SECURITY_STEP_ROLLBACK_DEINSTALLATION_SECURITY_H_

#include <manifest_parser/utils/logging.h>

#include "common/step/step.h"

namespace common_installer {
namespace security {

// Step that is used during uninstallation to rollback security changes if
// deinstallation fails
__attribute__ ((visibility ("default"))) class StepRollbackDeinstallationSecurity : public Step {
 public:
  using Step::Step;

  Status process() override { return Status::OK; }
  Status undo() override;
  Status clean() override { return Status::OK; }
  Status precheck() override;

  SCOPE_LOG_TAG(RollbackDeinstallationSecurity)
};

}  // namespace security
}  // namespace common_installer

#endif  // COMMON_STEP_SECURITY_STEP_ROLLBACK_DEINSTALLATION_SECURITY_H_
