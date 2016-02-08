// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_REMOVE_MANIFEST_H_
#define COMMON_STEP_REMOVE_MANIFEST_H_

#include <manifest_parser/utils/logging.h>

#include "common/installer_context.h"
#include "common/step/step.h"

namespace common_installer {
namespace pkgmgr {

/**
 * \brief Step responsbile for removing manifest file during uninstallation
 */
class StepRemoveManifest : public Step {
 public:
  using Step::Step;

  /**
   * \brief main logic of remove manifest
   *
   * \return Status::OK if success, Status::ERROR otherwise
   */
  Status process() override;

  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override { return Status::OK; }

  SCOPE_LOG_TAG(RemoveManifest)
};

}  // namespace pkgmgr
}  // namespace common_installer

#endif  // COMMON_STEP_REMOVE_MANIFEST_H_
