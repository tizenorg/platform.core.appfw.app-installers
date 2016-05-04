// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_FILESYSTEM_STEP_CLEAR_DATA_H_
#define COMMON_STEP_FILESYSTEM_STEP_CLEAR_DATA_H_

#include <string>

#include "common/installer_context.h"
#include "common/step/step.h"
#include "common/utils/logger.h"

namespace common_installer {
namespace filesystem {

/**
 * \brief Step responsible removing data/ directory in pkgmgr clear request
 *        Used by WGT and TPK backend
 */
class StepClearData : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status undo() override { return Status::OK; }
  Status clean() override { return Status::OK; }
  Status precheck() override;

  SCOPE_LOG_TAG(ClearData)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_FILESYSTEM_STEP_CLEAR_DATA_H_
