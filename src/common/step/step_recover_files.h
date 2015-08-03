// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_RECOVER_FILES_H_
#define COMMON_STEP_STEP_RECOVER_FILES_H_

#include "common/context_installer.h"
#include "common/step/step_recovery.h"
#include "common/utils/logging.h"

namespace common_installer {
namespace filesystem {

/**
 * @brief The StepRecoverBackup class
 *
 * Fixes state of package files in recovery mode.
 * For recovering new installation, package files are removed.
 * For recovering update installation, old package files are restored to its
 * original locations.
 */
class StepRecoverFiles : public recovery::StepRecovery {
 public:
  using StepRecovery::StepRecovery;

  Status RecoveryNew() override;
  Status RecoveryUpdate() override;

 private:
  bool SetPackagePath();

  SCOPE_LOG_TAG(RecoverBackup)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_RECOVER_FILES_H_
