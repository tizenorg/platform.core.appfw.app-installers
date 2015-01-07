// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_SIGNATURE_H_
#define COMMON_STEP_STEP_SIGNATURE_H_

#include "common/context_installer.h"
#include "common/step/step.h"

namespace common_installer {
namespace signature {

class StepSignature : public Step {
 public:
  using Step::Step;

  Step::Status process() override;
  Step::Status undo() override { return Step::Status::OK; }
  Step::Status clean() override { return Step::Status::OK; }
};

}  // namespace signature
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_SIGNATURE_H_
