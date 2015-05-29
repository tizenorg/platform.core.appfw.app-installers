// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_CREATE_STORAGE_DIRECTORIES_H_
#define COMMON_STEP_STEP_CREATE_STORAGE_DIRECTORIES_H_

#include "common/step/step.h"
#include "utils/logging.h"

namespace common_installer {
namespace create_storage {

class StepCreateStorageDirectories : public common_installer::Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override { return Status::OK; }

  SCOPE_LOG_TAG(CreateStorageDirectories)
};

}  // namespace create_storage
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_CREATE_STORAGE_DIRECTORIES_H_
