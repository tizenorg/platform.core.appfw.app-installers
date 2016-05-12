// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_CONFIGURATION_STEP_BLOCK_CROSS_UPDATE_H_
#define COMMON_STEP_CONFIGURATION_STEP_BLOCK_CROSS_UPDATE_H_

#include <manifest_parser/utils/logging.h>

#include "common/installer_context.h"

#include "common/step/step.h"

namespace common_installer {
namespace configuration {

/**
 * @brief The StepBlockCrossUpdate class
 *        This step is used to block update of package in case when installation
 *        was performed in different way than update.
 *
 * Packages that where installed by mount-install cannot be update with normal
 * installation and packages that where installed by normal installation cannot
 * be updated with mount update.
 *
 * We cannot use reinstall, RDS and delta update with mount-installed package
 * in current version.
 */
class StepBlockCrossUpdate : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override;

  SCOPE_LOG_TAG(BlockCrossUpdate)
};

}  // namespace configuration
}  // namespace common_installer

#endif  // COMMON_STEP_CONFIGURATION_STEP_BLOCK_CROSS_UPDATE_H_
