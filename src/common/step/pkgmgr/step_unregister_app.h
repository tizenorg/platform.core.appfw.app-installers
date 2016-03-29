// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_PKGMGR_STEP_UNREGISTER_APP_H_
#define COMMON_STEP_PKGMGR_STEP_UNREGISTER_APP_H_

#include <manifest_parser/utils/logging.h>

#include "common/installer_context.h"
#include "common/step/step.h"

namespace common_installer {
namespace pkgmgr {

class StepUnregisterApplication : public Step {
 public:
  using Step::Step;

  __attribute__ ((visibility ("default"))) Step::Status process() override;
  __attribute__ ((visibility ("default"))) Step::Status clean() override { return Status::OK; }
  __attribute__ ((visibility ("default"))) Step::Status undo() override;
  __attribute__ ((visibility ("default"))) Status precheck() override;

 private:
  __attribute__ ((visibility ("default"))) bool BackupCertInfo();

  SCOPE_LOG_TAG(Unregister)
};

}  // namespace pkgmgr
}  // namespace common_installer

#endif  // COMMON_STEP_PKGMGR_STEP_UNREGISTER_APP_H_
