// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_FILESYSTEM_STEP_DELTA_PATCH_H_
#define COMMON_STEP_FILESYSTEM_STEP_DELTA_PATCH_H_

#include <manifest_parser/utils/logging.h>

#include <string>

#include "common/installer_context.h"
#include "common/step/step.h"

namespace common_installer {
namespace filesystem {

/**
 * @brief The StepDeltaPatch class
 *        Patches the unpack directory content according to delta file so that
 *        new package content is complete and no file is missing.
 *
 * Patching package files with delta is performed in unpacked_dir before all
 * step that operates on widget's final location. That means that whole new
 * package content is prepared before any files of package installation are
 * touched.
 *
 * Flow goes as below:
 *  1) `unpacked_dir` (where delta package is unpacked) is moved to
 *     `unpacked_dir_patch` (PATCH DIR)
 *  2) old package content is moved to `unpacked_dir` (it becomes APP_DIR)
 *  3) `unpacked_dir` is being applied with patches (using xdelta3 tool)
 *  4) `unpacked_dir_patch` is removed.
 *  5) Normal update flow proceeds as if it it was normal update installation
 *     (as the unpacked_dir contains full new version of package).
 */
class StepDeltaPatch : public Step {
 public:
  __attribute__ ((visibility ("default"))) explicit StepDeltaPatch(InstallerContext* context,
                          const std::string& delta_root = "");

  __attribute__ ((visibility ("default"))) Status process() override;
  __attribute__ ((visibility ("default"))) Status clean() override { return Status::OK; }
  __attribute__ ((visibility ("default"))) Status undo() override;
  __attribute__ ((visibility ("default"))) Status precheck() override;

 private:
  boost::filesystem::path patch_dir_;
  std::string delta_root_;

  SCOPE_LOG_TAG(DeltaPatch)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_FILESYSTEM_STEP_DELTA_PATCH_H_
