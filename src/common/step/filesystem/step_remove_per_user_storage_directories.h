// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_FILESYSTEM_STEP_REMOVE_PER_USER_STORAGE_DIRECTORIES_H_
#define COMMON_STEP_FILESYSTEM_STEP_REMOVE_PER_USER_STORAGE_DIRECTORIES_H_

#include <manifest_parser/utils/logging.h>

#include "common/step/step.h"

namespace common_installer {
namespace filesystem {

/**
 * \brief Uninstallation.
 *        Responsible for removing shared directories (wgt/tpk) for users.
 *
 * * process method implements removal of data and shared directories for
 *   package in case of global installation for each user. Normally, directories
 *   are removed in StepRemoveFiles.
 *
 * * Other methods are empty.
 *
 */

class StepRemovePerUserStorageDirectories : public common_installer::Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override { return Status::OK; }

  STEP_NAME(RemovePerUserStorageDirectories)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_FILESYSTEM_STEP_REMOVE_PER_USER_STORAGE_DIRECTORIES_H_
