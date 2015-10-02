// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_FILESYSTEM_STEP_ACQUIRE_PACKAGE_STORAGE_H_
#define COMMON_STEP_FILESYSTEM_STEP_ACQUIRE_PACKAGE_STORAGE_H_

#include "common/installer_context.h"
#include "common/step/step.h"

namespace common_installer {
namespace filesystem {

/**
 * \brief Creates package storage according to:
 *        1) package installation
 *        2) manifest file
 */
class StepAcquirePackageStorage : public Step {
 public:
  using Step::Step;

  Status process() override;

  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override;

  SCOPE_LOG_TAG(AcquirePackageStorage)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_FILESYSTEM_STEP_ACQUIRE_PACKAGE_STORAGE_H_
