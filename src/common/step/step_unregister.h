// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_UNREGISTER_H_
#define COMMON_STEP_STEP_UNREGISTER_H_

#include "common/context_installer.h"
#include "common/step/step.h"

namespace common_installer {
namespace unregister {

class StepUnregister : public Step {
 public:
  using Step::Step;

  Step::Status process() override;
  Step::Status clean() override;
  Step::Status undo() override;
};

}  // namespace unregister
}  // namespace common_installer


#endif  // COMMON_STEP_STEP_UNREGISTER_H_
