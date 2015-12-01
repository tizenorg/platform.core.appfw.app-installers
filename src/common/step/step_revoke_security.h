// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_REVOKE_SECURITY_H_
#define COMMON_STEP_STEP_REVOKE_SECURITY_H_

#include <manifest_parser/utils/logging.h>

#include "common/step/step.h"

namespace common_installer {
namespace security {

// Step that is used during uninstallation
// Security rules are revoked on cleanup as until that point we need to keep
// package files present for rollback
class StepRevokeSecurity : public Step {
 public:
  using Step::Step;

  Status process() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status clean() override;
  Status precheck() override;

  SCOPE_LOG_TAG(RevokeSecurity)
};

}  // namespace security
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_REVOKE_SECURITY_H_
