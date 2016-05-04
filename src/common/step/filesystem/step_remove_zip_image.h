// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_FILESYSTEM_STEP_REMOVE_ZIP_IMAGE_H_
#define COMMON_STEP_FILESYSTEM_STEP_REMOVE_ZIP_IMAGE_H_

#include "common/installer_context.h"

#include "common/step/step.h"
#include "common/utils/logger.h"

namespace common_installer {
namespace filesystem {

/**
 * @brief The StepRemoveZipImage class
 *        This step removes zip file from directory if it exists.
 *
 * Mount installation will leave zip file outside package path
 */
class StepRemoveZipImage : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override  { return Status::OK; }
  Status precheck() override;

  SCOPE_LOG_TAG(RemoveZipImage)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_FILESYSTEM_STEP_REMOVE_ZIP_IMAGE_H_
