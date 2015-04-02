// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_BACKUP_MANIFEST_H_
#define COMMON_STEP_STEP_BACKUP_MANIFEST_H_

#include "common/context_installer.h"
#include "common/step/step.h"
#include "utils/logging.h"

namespace common_installer {
namespace backup_manifest {

class StepBackupManifest : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override;
  Status undo() override;
  Status precheck() override;

  SCOPE_LOG_TAG(BackupManifest)
};

}  // namespace backup_manifest
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_BACKUP_MANIFEST_H_