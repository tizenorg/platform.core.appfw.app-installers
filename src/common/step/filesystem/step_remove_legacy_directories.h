// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_FILESYSTEM_STEP_REMOVE_LEGACY_DIRECTORIES_H_
#define COMMON_STEP_FILESYSTEM_STEP_REMOVE_LEGACY_DIRECTORIES_H_

#include <manifest_parser/utils/logging.h>

#include "common/step/step.h"

namespace common_installer {
namespace filesystem {

/**
 * \brief Uninstallation.
 *        Responsible for removing RW directoires under legacy app-root
 *        (wgt/tpk)
 *
 * * process method implements removal of data and shared directories
 *   under /opt/usr/apps.
 *
 * * Other methods are empty.
 *
 */

class StepRemoveLegacyDirectories : public common_installer::Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override { return Status::OK; }

  STEP_NAME(RemoveLegacyDirectories)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_FILESYSTEM_STEP_REMOVE_LEGACY_DIRECTORIES_H_
