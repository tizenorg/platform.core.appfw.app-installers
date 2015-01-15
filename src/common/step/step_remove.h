// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_REMOVE_H_
#define COMMON_STEP_STEP_REMOVE_H_

#include "common/context_installer.h"

#include "common/step/step.h"

namespace common_installer {
namespace remove {

class StepRemove : public Step {
 public:
  int process(ContextInstaller* context) override;
  int clean(ContextInstaller* context) override;
  int undo(ContextInstaller* context) override;
};

}  // namespace remove
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_REMOVE_H_
