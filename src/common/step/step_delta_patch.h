// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_DELTA_PATCH_H_
#define COMMON_STEP_STEP_DELTA_PATCH_H_

#include <string>

#include "common/installer_context.h"
#include "common/step/step.h"
#include "common/utils/logging.h"

namespace common_installer {
namespace filesystem {

/**
 * @brief The StepDeltaPatch class
 *        Patches the unpack directory content according to delta file so that
 *        new package content is complete and no file is missing.
 */
class StepDeltaPatch : public Step {
 public:
  explicit StepDeltaPatch(InstallerContext* context,
                          const std::string& delta_root = "");

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override;
  Status precheck() override;

 private:
  boost::filesystem::path patch_dir_;
  std::string delta_root_;

  SCOPE_LOG_TAG(DeltaPatch)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_DELTA_PATCH_H_
