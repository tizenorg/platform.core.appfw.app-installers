// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_STEP_CHECK_SETTINGS_LEVEL_H_
#define WGT_STEP_STEP_CHECK_SETTINGS_LEVEL_H_

#include "common/app_installer.h"
#include "common/context_installer.h"
#include "common/step/step.h"
#include "common/utils/logging.h"

namespace wgt {
namespace security {

class StepCheckSettingsLevel : public common_installer::Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override { return Status::OK; }

  SCOPE_LOG_TAG(CheckSettingsLevel)
};

}  // namespace security
}  // namespace wgt

#endif  // WGT_STEP_STEP_CHECK_SETTINGS_LEVEL_H_
