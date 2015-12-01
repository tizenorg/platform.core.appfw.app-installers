// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_RECOVER_SECURITY_H_
#define COMMON_STEP_STEP_RECOVER_SECURITY_H_

#include "common/installer_context.h"
#include "common/step/step_recovery.h"
#include "manifest_parser/utils/logging.h"

namespace common_installer {
namespace security {

/**
 * @brief responsible for restoring security
 *        rules
 *
 * Part of Recovery Mode. In case of partial
 * installation security rules get unregistered
 * In case of unsuccessful partial update
 * security rules for the app before update
 * gets restored
 */
class StepRecoverSecurity : public recovery::StepRecovery {
 public:
  using StepRecovery::StepRecovery;

  Status RecoveryNew() override;
  Status RecoveryUpdate() override;
 private:
  bool Check();

  SCOPE_LOG_TAG(RecoverSecurity)
};

}  // namespace security
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_RECOVER_SECURITY_H_
