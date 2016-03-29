// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_SECURITY_STEP_REGISTER_SECURITY_H_
#define COMMON_STEP_SECURITY_STEP_REGISTER_SECURITY_H_

#include <manifest_parser/utils/logging.h>

#include "common/step/step.h"

namespace common_installer {
namespace security {

class StepRegisterSecurity : public Step {
 public:
  using Step::Step;

  __attribute__ ((visibility ("default"))) Status process() override;
  __attribute__ ((visibility ("default"))) Status undo() override { return Status::OK; }
  __attribute__ ((visibility ("default"))) Status clean() override { return Status::OK; }
  __attribute__ ((visibility ("default"))) Status precheck() override;

  SCOPE_LOG_TAG(Security)
};

}  // namespace security
}  // namespace common_installer

#endif  // COMMON_STEP_SECURITY_STEP_REGISTER_SECURITY_H_
