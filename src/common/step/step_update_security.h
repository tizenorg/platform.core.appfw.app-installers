// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_UPDATE_SECURITY_H_
#define COMMON_STEP_STEP_UPDATE_SECURITY_H_

#include "common/step/step.h"
#include "utils/logging.h"

namespace common_installer {
namespace update_security {

class StepUpdateSecurity : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status undo() override;
  Status clean() override { return Status::OK; }
  Status precheck() override { return Status::OK; }

  SCOPE_LOG_TAG(UpdateSecurity)
};

}  // namespace update_security
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_UPDATE_SECURITY_H_
