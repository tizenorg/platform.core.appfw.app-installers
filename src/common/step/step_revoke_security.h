// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_REVOKE_SECURITY_H_
#define COMMON_STEP_STEP_REVOKE_SECURITY_H_

#include "common/step/step.h"
#include "utils/logging.h"

namespace common_installer {
namespace revoke_security {

// Step that is used during uninstallation
class StepRevokeSecurity : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status undo() override;
  Status clean() override { return Status::OK; }

  SCOPE_LOG_TAG(RevokeSecurity)
};

}  // namespace revoke_security
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_REVOKE_SECURITY_H_
