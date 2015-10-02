// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_UPDATE_APP_H_
#define COMMON_STEP_STEP_UPDATE_APP_H_

#include "common/installer_context.h"
#include "common/step/step.h"
#include "common/utils/logging.h"

namespace common_installer {
namespace pkgmgr {

class StepUpdateApplication : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override;
  Status precheck() override;

  SCOPE_LOG_TAG(UpdateApplication)
};

}  // namespace pkgmgr
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_UPDATE_APP_H_
