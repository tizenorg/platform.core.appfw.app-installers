// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_UNREGISTER_APP_H_
#define COMMON_STEP_STEP_UNREGISTER_APP_H_

#include "common/context_installer.h"
#include "common/step/step.h"
#include "common/utils/logging.h"

namespace common_installer {
namespace pkgmgr {

class StepUnregisterApplication : public Step {
 public:
  using Step::Step;

  Step::Status process() override;
  Step::Status clean() override { return Status::OK; }
  Step::Status undo() override;
  Status precheck() override;

 private:
  void BackupCertInfo();

  SCOPE_LOG_TAG(Unregister)
};

}  // namespace pkgmgr
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_UNREGISTER_APP_H_
