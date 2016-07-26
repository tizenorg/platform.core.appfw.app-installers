// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_FILESYSTEM_STEP_REMOVE_GLOBALAPP_SYMLINKS_H_
#define COMMON_STEP_FILESYSTEM_STEP_REMOVE_GLOBALAPP_SYMLINKS_H_

#include <manifest_parser/utils/logging.h>

#include "common/step/step.h"

namespace common_installer {
namespace filesystem {

/**
 * \brief Uninstallation.
 *        Responsible for removing symlink files for read-only files of globalapp
 *        for backward compatibility. (wgt/tpk)
 *
 * * process method implements removal of symlinks under RW app_root per user.
 *
 * * Other methods are empty.
 *
 */

class StepRemoveGlobalAppSymlinks : public common_installer::Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override { return Status::OK; }

  STEP_NAME(RemoveGlobalAppSymlinks)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_FILESYSTEM_STEP_REMOVE_GLOBALAPP_SYMLINKS_H_
