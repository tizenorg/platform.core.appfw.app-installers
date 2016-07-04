/* 2016, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_FILESYSTEM_STEP_CHANGE_OWNER_H_
#define COMMON_STEP_FILESYSTEM_STEP_CHANGE_OWNER_H_

#include <manifest_parser/utils/logging.h>

#include "common/installer_context.h"

#include "common/step/step.h"

namespace common_installer {
namespace filesystem {

/**
 * \brief step responsible for changing ownership from system uid to actual user.
 *        Used by WGT and TPK
 */
class StepChangeOwner : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override;
  STEP_NAME(ChangeOwner)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_FILESYSTEM_STEP_CHANGE_OWNER_H_
