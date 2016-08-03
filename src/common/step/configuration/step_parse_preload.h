// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_CONFIGURATION_STEP_PARSE_PRELOAD_H_
#define COMMON_STEP_CONFIGURATION_STEP_PARSE_PRELOAD_H_

#include <manifest_parser/utils/logging.h>

#include "common/installer_context.h"
#include "common/step/step.h"

namespace common_installer {
namespace configuration {

class StepParsePreload : public common_installer::Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override { return Status::OK; }

  STEP_NAME(ParsePreload)
};

}  // namespace configuration
}  // namespace common_installer

#endif  // COMMON_STEP_CONFIGURATION_STEP_PARSE_PRELOAD_H_
