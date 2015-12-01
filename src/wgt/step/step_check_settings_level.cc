// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_check_settings_level.h"

#include <manifest_handlers/setting_handler.h>
#include <manifest_parser/utils/logging.h>

#include <map>

#include "wgt/wgt_backend_data.h"

namespace {

bool ValidateSettingsForLevel(common_installer::PrivilegeLevel level,
                              const wgt::parse::SettingInfo& settings) {
  if (settings.background_vibration()) {
    common_installer::PrivilegeLevel required_level =
        common_installer::PrivilegeLevel::PARTNER;
    if (!common_installer::SatifiesPrivilegeLevel(required_level, level)) {
      LOG(ERROR) << "background_vibration requires visibility level: "
                 << common_installer::PrivilegeLevelToString(required_level);
      return false;
    }
    LOG(INFO) << "Setting: 'background-vibration' allowed";
  }
  return true;
}

}  // namespace

namespace wgt {
namespace security {

common_installer::Step::Status StepCheckSettingsLevel::process() {
  if (!ValidateSettingsForLevel(context_->privilege_level.get(),
      static_cast<WgtBackendData*>(
          context_->backend_data.get())->settings.get())) {
    return Status::ERROR;
  }
  LOG(INFO) << "Settings privilege level checked";
  return Status::OK;
}

}  // namespace security
}  // namespace wgt
