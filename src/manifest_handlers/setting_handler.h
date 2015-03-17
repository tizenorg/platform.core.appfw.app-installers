// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef MANIFEST_HANDLERS_SETTING_HANDLER_H_
#define MANIFEST_HANDLERS_SETTING_HANDLER_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "parser/values.h"
#include "parser/manifest_handler.h"

namespace wgt {
namespace parse {

class SettingInfo : public parser::ManifestData {
 public:
  SettingInfo();
  virtual ~SettingInfo();

  enum ScreenOrientation {
    PORTRAIT,
    LANDSCAPE,
    AUTO
  };

  void set_hwkey_enabled(bool enabled) { hwkey_enabled_ = enabled; }
  bool hwkey_enabled() const { return hwkey_enabled_; }

  void set_screen_orientation(ScreenOrientation orientation) {
    screen_orientation_ = orientation;
  }

  ScreenOrientation screen_orientation() const { return screen_orientation_; }

  void set_encryption_enabled(bool enabled) { encryption_enabled_ = enabled; }
  bool encryption_enabled() const { return encryption_enabled_; }

  void set_context_menu_enabled(bool enabled) {
    context_menu_enabled_ = enabled;
  }
  bool context_menu_enabled() const { return context_menu_enabled_; }

  void set_background_support_enabled(bool enabled) {
    background_support_enabled_ = enabled;
  }
  bool background_support_enabled() const {
    return background_support_enabled_;
  }

 private:
  bool hwkey_enabled_;
  ScreenOrientation screen_orientation_;
  bool encryption_enabled_;
  bool context_menu_enabled_;
  bool background_support_enabled_;
};

class SettingHandler : public parser::ManifestHandler {
 public:
  SettingHandler();
  virtual ~SettingHandler();

  bool Parse(
      const parser::Manifest& manifest,
      std::shared_ptr<parser::ManifestData>* output,
      std::string* error) override;
  bool Validate(
      const parser::ManifestData& application,
      const parser::ManifestDataMap& handlers_output,
      std::string* error) const override;
  std::string Key() const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(SettingHandler);
};

}  // namespace parse
}  // namespace wgt

#endif  // MANIFEST_HANDLERS_SETTING_HANDLER_H_
