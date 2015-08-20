// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_PRIVILEGE_COMPATIBILITY_H_
#define COMMON_STEP_STEP_PRIVILEGE_COMPATIBILITY_H_

#include "common/context_installer.h"
#include "common/step/step.h"
#include "common/utils/logging.h"

namespace common_installer {
namespace security {

/**
 * @brief This step converts privileges declared in package manifest to the
 *        privileges for current platform version
 */
class StepPrivilegeCompatibility : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  // backward translation not needed
  Status undo() override { return Status::OK; }
  Status precheck() override;

  SCOPE_LOG_TAG(PrivilegeCompatibility)
};

}  // namespace security
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_PRIVILEGE_COMPATIBILITY_H_