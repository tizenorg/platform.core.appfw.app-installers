// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_RECOVER_STORAGE_DIRECTORIES_H_
#define COMMON_STEP_STEP_RECOVER_STORAGE_DIRECTORIES_H_

#include "common/context_installer.h"
#include "common/step/step_recovery.h"
#include "common/utils/logging.h"

namespace common_installer {
namespace filesystem {

/**
 * @brief recovers data and shared directories of application
 *
 * Part of Recovery Mode. Is responsible for restoring data and shared
 * directories in case partial update occurs.
 */
class StepRecoverStorageDirectories : public recovery::StepRecovery {
 public:
  using StepRecovery::StepRecovery;

  Status RecoveryNew() override { return Status::OK; }
  Status RecoveryUpdate() override;
 private:
  /**
   * @brief moves directories
   * @param in_src path of the source location
   * @param in_dst path of the destination location
   * @param key directory name
   * @return true if operation successful
   */
  bool MoveAppStorage(const boost::filesystem::path& in_src,
                      const boost::filesystem::path& in_dst,
                      const char *key);

  SCOPE_LOG_TAG(RecoverStorageDirectories)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_RECOVER_STORAGE_DIRECTORIES_H_
