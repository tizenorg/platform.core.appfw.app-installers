// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include <manifest_handlers/setting_handler.h>

#include "common/installer_context.h"
#include "wgt/step/step_check_wgt_background_category.h"
#include "wgt/wgt_backend_data.h"

namespace {
namespace ci_sec = common_installer::security;
}  // namespace

namespace wgt {
namespace security {

StepCheckWgtBackgroundCategory::StepCheckWgtBackgroundCategory(
    common_installer::InstallerContext* context) :
        ci_sec::StepCheckBackgroundCategory(context) {}

bool StepCheckWgtBackgroundCategory::GetBackgroundSupport() {
  const wgt::parse::SettingInfo& settings = static_cast<WgtBackendData*>(
      context_->backend_data.get())->settings.get();
  return settings.background_support_enabled();
}

}  // namespace security
}  // namespace wgt
