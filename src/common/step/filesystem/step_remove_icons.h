// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_FILESYSTEM_STEP_REMOVE_ICONS_H_
#define COMMON_STEP_FILESYSTEM_STEP_REMOVE_ICONS_H_

#include <boost/filesystem/path.hpp>

#include <manifest_parser/utils/logging.h>

#include <utility>
#include <vector>

#include "common/installer_context.h"
#include "common/step/step.h"

namespace common_installer {
namespace filesystem {

class StepRemoveIcons : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override;

  STEP_NAME(RemoveIcons)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_FILESYSTEM_STEP_REMOVE_ICONS_H_
