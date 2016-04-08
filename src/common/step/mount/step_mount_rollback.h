// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_MOUNT_STEP_MOUNT_ROLLBACK_H_
#define COMMON_STEP_MOUNT_STEP_MOUNT_ROLLBACK_H_

#include <manifest_parser/utils/logging.h>

#include "common/installer_context.h"
#include "common/step/step.h"

namespace common_installer {
namespace mount {

/**
 * \brief Responsible for mounting old package zip in package installation
 *        directory in case of rollback for enable old content for rollback
 *        operations.
 */
class StepMountRollback : public Step {
 public:
  using Step::Step;

  Status process() override { return Status::OK; }
  Status clean() override { return Status::OK; }
  Status undo() override;
  Status precheck() override { return Status::OK; }

  SCOPE_LOG_TAG(MountRollback)
};

}  // namespace mount
}  // namespace common_installer

#endif  // COMMON_STEP_MOUNT_STEP_MOUNT_ROLLBACK_H_
