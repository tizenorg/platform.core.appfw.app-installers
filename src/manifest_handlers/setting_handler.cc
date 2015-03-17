// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "manifest_handlers/setting_handler.h"

#include <string.h>

#include <cassert>
#include <map>
#include <utility>

#include "manifest_handlers/application_manifest_constants.h"

namespace wgt {
namespace parse {

namespace keys = wgt::application_widget_keys;

SettingInfo::SettingInfo()
    : hwkey_enabled_(true),
      screen_orientation_(PORTRAIT),
      background_support_enabled_(false) {}

SettingInfo::~SettingInfo() {}

SettingHandler::SettingHandler() {}

SettingHandler::~SettingHandler() {}

bool SettingHandler::Parse(
    const parser::Manifest& manifest,
    std::shared_ptr<parser::ManifestData>* output,
    std::string* /*error*/) {
  std::shared_ptr<SettingInfo> app_info(new SettingInfo);
  std::string hwkey;
  manifest.GetString(keys::kTizenHardwareKey, &hwkey);
  app_info->set_hwkey_enabled(hwkey != "disable");

  std::string screen_orientation;
  manifest.GetString(keys::kTizenScreenOrientationKey, &screen_orientation);
  if (strcasecmp("portrait", screen_orientation.c_str()) == 0)
    app_info->set_screen_orientation(SettingInfo::PORTRAIT);
  else if (strcasecmp("landscape", screen_orientation.c_str()) == 0)
    app_info->set_screen_orientation(SettingInfo::LANDSCAPE);
  else
    app_info->set_screen_orientation(SettingInfo::AUTO);
  std::string encryption;
  manifest.GetString(keys::kTizenEncryptionKey, &encryption);
  app_info->set_encryption_enabled(encryption == "enable");

  std::string context_menu;
  manifest.GetString(keys::kTizenContextMenuKey, &context_menu);
  app_info->set_context_menu_enabled(context_menu != "disable");

  std::string background_support;
  manifest.GetString(keys::kTizenBackgroundSupportKey, &background_support);
  app_info->set_background_support_enabled(background_support == "enable");

  *output = std::static_pointer_cast<parser::ManifestData>(app_info);
  return true;
}

bool SettingHandler::Validate(
    const parser::ManifestData& data,
    const parser::ManifestDataMap& /*handlers_output*/,
    std::string* error) const {
  const SettingInfo& setting_info =
      static_cast<const SettingInfo&>(data);
  if (setting_info.screen_orientation() !=
          SettingInfo::ScreenOrientation::AUTO &&
      setting_info.screen_orientation() !=
          SettingInfo::ScreenOrientation::PORTRAIT &&
      setting_info.screen_orientation() !=
          SettingInfo::ScreenOrientation::LANDSCAPE) {
    *error = "Wrong value of screen orientation";
    return false;
  }
  return true;
}

std::string SettingHandler::Key() const {
  return keys::kTizenSettingKey;
}

}  // namespace parse
}  // namespace wgt
