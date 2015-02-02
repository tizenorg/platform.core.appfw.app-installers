// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "widget-manifest-parser/manifest_handlers/setting_handler.h"

#include <cassert>
#include <map>
#include <utility>

#include "widget-manifest-parser/application_manifest_constants.h"

namespace common_installer {
namespace widget_manifest_parser {

namespace keys = application_widget_keys;

SettingInfo::SettingInfo()
    : hwkey_enabled_(true),
      screen_orientation_(PORTRAIT),
      background_support_enabled_(false) {}

SettingInfo::~SettingInfo() {}

SettingHandler::SettingHandler() {}

SettingHandler::~SettingHandler() {}

bool SettingHandler::Parse(std::shared_ptr<ApplicationData> application,
                                std::string* /*error*/) {
  std::shared_ptr<SettingInfo> app_info(new SettingInfo);
  const Manifest* manifest = application->GetManifest();
  assert(manifest);

  std::string hwkey;
  manifest->GetString(keys::kTizenHardwareKey, &hwkey);
  app_info->set_hwkey_enabled(hwkey != "disable");

  std::string screen_orientation;
  manifest->GetString(keys::kTizenScreenOrientationKey, &screen_orientation);
  if (strcasecmp("portrait", screen_orientation.c_str()) == 0)
    app_info->set_screen_orientation(SettingInfo::PORTRAIT);
  else if (strcasecmp("landscape", screen_orientation.c_str()) == 0)
    app_info->set_screen_orientation(SettingInfo::LANDSCAPE);
  else
    app_info->set_screen_orientation(SettingInfo::AUTO);
  std::string encryption;
  manifest->GetString(keys::kTizenEncryptionKey, &encryption);
  app_info->set_encryption_enabled(encryption == "enable");

  std::string context_menu;
  manifest->GetString(keys::kTizenContextMenuKey, &context_menu);
  app_info->set_context_menu_enabled(context_menu != "disable");

  std::string background_support;
  manifest->GetString(keys::kTizenBackgroundSupportKey, &background_support);
  app_info->set_background_support_enabled(background_support == "enable");

  application->SetManifestData(keys::kTizenSettingKey, app_info);
  return true;
}

bool SettingHandler::Validate(
    std::shared_ptr<const ApplicationData> application,
    std::string* error) const {
  const Manifest* manifest = application->GetManifest();
  assert(manifest);
  std::string hwkey;
  manifest->GetString(keys::kTizenHardwareKey, &hwkey);
  if (!hwkey.empty() && hwkey != "enable" && hwkey != "disable") {
    *error = std::string("The hwkey value must be 'enable'/'disable',"
                         " or not specified in configuration file.");
    return false;
  }

  std::string screen_orientation;
  manifest->GetString(keys::kTizenScreenOrientationKey, &screen_orientation);
  if (!screen_orientation.empty() &&
      strcasecmp("portrait", screen_orientation.c_str()) != 0 &&
      strcasecmp("landscape", screen_orientation.c_str()) != 0 &&
      strcasecmp("auto-rotation", screen_orientation.c_str()) != 0) {
    *error = std::string("The screen-orientation must be 'portrait'/"
                         "'landscape'/'auto-rotation' or not specified.");
    return false;
  }
  std::string encryption;
  manifest->GetString(keys::kTizenEncryptionKey, &encryption);
  if (!encryption.empty() && encryption != "enable" &&
      encryption != "disable") {
    *error = std::string("The encryption value must be 'enable'/'disable', "
                         "or not specified in configuration file.");
    return false;
  }
  std::string context_menu;
  manifest->GetString(keys::kTizenContextMenuKey, &context_menu);
  if (!context_menu.empty() &&
      context_menu != "enable" &&
      context_menu != "disable") {
    *error = std::string("The context-menu value must be 'enable'/'disable', "
                         "or not specified in configuration file.");
    return false;
  }
  std::string background_support;
  manifest->GetString(keys::kTizenBackgroundSupportKey, &background_support);
  if (!background_support.empty() &&
      background_support != "enable" &&
      background_support != "disable") {
    *error = std::string("The background-support value must be"
                         "'enable'/'disable', or not specified in configuration"
                         "file.");
  }
  return true;
}

std::vector<std::string> SettingHandler::Keys() const {
  return std::vector<std::string>(1, keys::kTizenSettingKey);
}

}  // namespace widget_manifest_parser
}  // namespace common_installer
