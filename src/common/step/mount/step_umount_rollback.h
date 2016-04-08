// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_MOUNT_STEP_UMOUNT_ROLLBACK_H_
#define COMMON_STEP_MOUNT_STEP_UMOUNT_ROLLBACK_H_

#include <manifest_parser/utils/logging.h>

#include "common/installer_context.h"
#include "common/step/step.h"

namespace common_installer {
namespace mount {

/**
 * \brief Responsible for umounting old package zip from package installation
 *        directory in case of rollback after StepMountRollback
 */
class StepUmountRollback : public Step {
 public:
  using Step::Step;

  Status process() override { return Status::OK; }
  Status clean() override { return Status::OK; }
  Status undo() override;
  Status precheck() override { return Status::OK; }

  SCOPE_LOG_TAG(UmountRollback)
};

}  // namespace mount
}  // namespace common_installer

#endif  // COMMON_STEP_MOUNT_STEP_UMOUNT_ROLLBACK_H_
