// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_FAIL_H_
#define COMMON_STEP_STEP_FAIL_H_

#include <manifest_parser/utils/logging.h>

#include "common/installer_context.h"

#include "common/step/step.h"

namespace common_installer {
namespace configuration {

/**
 * \brief step used to indicate wrong request provided to the backend.
 *        Used by TPK and WGT.
 */
class StepFail : public Step {
 public:
  using Step::Step;

  /**
   * \brief returns error
   *
   * \return Status::ERROR
   */
  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override { return Status::OK; }

  SCOPE_LOG_TAG(Fail)
};

}  // namespace configuration
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_FAIL_H_
