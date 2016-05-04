// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_FILESYSTEM_STEP_RECOVER_MANIFEST_H_
#define COMMON_STEP_FILESYSTEM_STEP_RECOVER_MANIFEST_H_

#include "common/installer_context.h"
#include "common/step/recovery/step_recovery.h"
#include "common/utils/logger.h"

namespace common_installer {
namespace filesystem {

/**
 * @brief The StepRecoverManifest class
 *        Fixes state of platform manfiest file in recovery mode.
 *
 * For recovering new installation, manifest file is removed.
 * For recovering update installation, the old manifest of package is restored
 * to its location.
 */
class StepRecoverManifest : public recovery::StepRecovery {
 public:
  using StepRecovery::StepRecovery;

  Status RecoveryNew() override;
  Status RecoveryUpdate() override;

 private:
  bool SetXmlPaths();

  SCOPE_LOG_TAG(RecoverManifest)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_FILESYSTEM_STEP_RECOVER_MANIFEST_H_
