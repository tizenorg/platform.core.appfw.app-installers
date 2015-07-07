// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_RECOVER_APPLICATION_H_
#define COMMON_STEP_STEP_RECOVER_APPLICATION_H_

#include "common/context_installer.h"
#include "common/step/step_recovery.h"

namespace common_installer {
namespace pkgmgr {

/**
 * @brief recovers applications entries in pkgmgr
 *
 * Part of RecoveryMode. In case of partial installation
 * app get unregistered with pkgmgr. In case of unsuccessful
 * update app in state before update gets re-registered
 */
class StepRecoverApplication : public recovery::StepRecovery {
 public:
  using StepRecovery::StepRecovery;
  Status RecoveryNew() override;
  Status RecoveryUpdate() override;
 private:
  bool SetXmlPaths();

  SCOPE_LOG_TAG(RecoverApplication)
};

}  // namespace pkgmgr
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_RECOVER_APPLICATION_H_