// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_check_background_category.h"

#include <manifest_handlers/setting_handler.h>

#include "common/installer_context.h"
#include "wgt/wgt_backend_data.h"

namespace wgt {
namespace security {

StepCheckBackgroundCategory::StepCheckBackgroundCategory(
    common_installer::InstallerContext *context) :
        BaseStepCheckBackgroundCategory(context) {};

bool StepCheckBackgroundCategory::GetBackgroundSupport() {
  const wgt::parse::SettingInfo& settings = static_cast<WgtBackendData*>(
      context_->backend_data.get())->settings.get();
  return settings.background_support_enabled();
}

}  // namespace security
}  // namespace wgt
