// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_BACKUP_STEP_BACKUP_MANIFEST_H_
#define COMMON_STEP_BACKUP_STEP_BACKUP_MANIFEST_H_

#include <manifest_parser/utils/logging.h>

#include "common/installer_context.h"
#include "common/step/step.h"

namespace common_installer {
namespace backup {

/**
 * \brief Step responsbile for backing manifest file during update and
 *        uninstallation. Used by WGT and TPK backend
 */
class __attribute__ ((visibility ("default"))) StepBackupManifest : public Step {
 public:
  using Step::Step;

  /**
   * \brief main logic of backuping manifest
   *
   * \return Status::OK if success, Status::ERROR otherwise
   */
  Status process() override;

  /**
   * \brief removes backup file after successful update/deinstallation
   *
   * \return Status::OK if success, Status::ERROR otherwise
   */
  Status clean() override;

  /**
   * \brief restores backup manifest.
   *
   * \return Status:OK if success, Status::ERROR othewise
   */
  Status undo() override;

  Status precheck() override;

  SCOPE_LOG_TAG(BackupManifest)
};

}  // namespace backup
}  // namespace common_installer

#endif  // COMMON_STEP_BACKUP_STEP_BACKUP_MANIFEST_H_
