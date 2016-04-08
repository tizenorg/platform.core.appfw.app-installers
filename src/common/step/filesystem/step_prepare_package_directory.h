// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_FILESYSTEM_STEP_PREPARE_PACKAGE_DIRECTORY_H_
#define COMMON_STEP_FILESYSTEM_STEP_PREPARE_PACKAGE_DIRECTORY_H_

#include <manifest_parser/utils/logging.h>

#include "common/installer_context.h"
#include "common/step/step.h"

namespace common_installer {
namespace filesystem {

/**
 * \brief Responsible for adjusting package directory after mounting zip package
 *        by StepMountInstall or StepMountUpdate
 *
 * This step will:
 *
 * 1) Extract partially content of zip package binaries must be extracted from:
 *    - bin/
 *    - lib/
 *
 * 2) Create symlinks for following files and directories to point them to
 *    mount path:
 *    - tizen-manifest.xml
 *    - author-signature.xml
 *    - signature*.xml
 *    - res/
 */
class StepPreparePackageDirectory : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override;
  Status undo() override;
  Status precheck() override;

  SCOPE_LOG_TAG(PreparePackageDirectory)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_FILESYSTEM_STEP_PREPARE_PACKAGE_DIRECTORY_H_
