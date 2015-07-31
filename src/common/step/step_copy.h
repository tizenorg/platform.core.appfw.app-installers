/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_COPY_H_
#define COMMON_STEP_STEP_COPY_H_

#include "common/context_installer.h"

#include "common/step/step.h"
#include "common/utils/logging.h"

namespace common_installer {
namespace filesystem {

class StepCopy : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override;
  Status precheck() override;

  SCOPE_LOG_TAG(Copy)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_COPY_H_
