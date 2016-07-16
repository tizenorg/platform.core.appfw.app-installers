// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_FILESYSTEM_STEP_CREATE_LEGACY_DIRECTORIES_H_
#define COMMON_STEP_FILESYSTEM_STEP_CREATE_LEGACY_DIRECTORIES_H_

#include <manifest_parser/utils/logging.h>

#include "common/step/step.h"

namespace common_installer {
namespace filesystem {

/**
 * \brief Installation.
 *        Responsible for creating RW directoires under legacy app-root
 *        for backward compatibility. (wgt/tpk)
 *
 * * process method implements creation of data and shared directories
 *   under /opt/usr/apps.
 *
 * * Other methods are empty.
 */
class StepCreateLegacyDirectories : public common_installer::Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override { return Status::OK; }

  STEP_NAME(CreateLegacyDirectories)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_FILESYSTEM_STEP_CREATE_LEGACY_DIRECTORIES_H_
