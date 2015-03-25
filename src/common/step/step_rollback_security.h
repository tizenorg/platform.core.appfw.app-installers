// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_ROLLBACK_SECURITY_H_
#define COMMON_STEP_STEP_ROLLBACK_SECURITY_H_

#include "common/step/step.h"
#include "utils/logging.h"

namespace common_installer {
namespace rollback_security {

class StepRollbackSecurity : public Step {
 public:
  using Step::Step;

  Status process() override { return Status::OK; }
  Status undo() override;
  Status clean() override { return Status::OK; }

  SCOPE_LOG_TAG(RollbackSecurity)
};

}  // namespace rollback_security
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_ROLLBACK_SECURITY_H_
