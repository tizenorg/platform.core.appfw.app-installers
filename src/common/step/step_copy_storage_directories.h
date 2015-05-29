// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_COPY_STORAGE_DIRECTORIES_H_
#define COMMON_STEP_STEP_COPY_STORAGE_DIRECTORIES_H_

#include "common/step/step.h"
#include "utils/logging.h"

namespace common_installer {
namespace copy_storage {

class StepCopyStorageDirectories : public common_installer::Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override { return Status::OK; }

  SCOPE_LOG_TAG(CreateStorageDirectories)
};

}  // namespace copy_storage
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_COPY_STORAGE_DIRECTORIES_H_
