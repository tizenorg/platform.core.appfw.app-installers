// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_FILESYSTEM_STEP_GRANT_PERMISSION_H_
#define COMMON_STEP_FILESYSTEM_STEP_GRANT_PERMISSION_H_

#include <manifest_parser/utils/logging.h>

#include <string>

#include "common/installer_context.h"
#include "common/step/step.h"

namespace common_installer {
namespace filesystem {

/**
 * \brief Step responsible for granting permissions to /bin, /lib
 */
class StepGrantPermission : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status undo() override { return Status::OK; }
  Status clean() override { return Status::OK; }
  Status precheck() override;

  SCOPE_LOG_TAG(GrantPermission)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_FILESYSTEM_STEP_GRANT_PERMISSION_H_
