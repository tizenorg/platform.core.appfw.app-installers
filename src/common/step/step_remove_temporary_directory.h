// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_REMOVE_TEMPORARY_DIRECTORY_H_
#define COMMON_STEP_STEP_REMOVE_TEMPORARY_DIRECTORY_H_

#include "common/step/step_recovery.h"
#include "common/utils/logging.h"

namespace common_installer {
namespace filesystem {

/**
 * @brief removes temp directories if install gets
 *        interrupted
 *
 * Part of Recovery Mode. If recovery mode gets called
 * path to temporary unpack directory and its content get
 * deleted
 */
class StepRemoveTemporaryDirectory : public recovery::StepRecovery {
 public:
  using StepRecovery::StepRecovery;

  Status RecoveryNew() override;
  Status RecoveryUpdate() override;
 private:
  /**
   * @brief RemoveFiles
   * Removes all the temporary files
   */
  void RemoveFiles();

  SCOPE_LOG_TAG(RemoveTemporaryDirectory)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_REMOVE_TEMPORARY_DIRECTORY_H_