// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.
// Part of this file is redistributed from crosswalk project.
// Check license file: LICENSE-xwalk

#ifndef WIDGET_MANIFEST_PARSER_MANIFEST_HANDLERS_SPLASH_SCREEN_HANDLER_H_
#define WIDGET_MANIFEST_PARSER_MANIFEST_HANDLERS_SPLASH_SCREEN_HANDLER_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "utils/values.h"
#include "widget-manifest-parser/application_data.h"
#include "widget-manifest-parser/manifest_handler.h"

namespace common_installer {
namespace widget_manifest_parser {

class SplashScreenInfo : public ApplicationData::ManifestData {
 public:
  SplashScreenInfo();
  virtual ~SplashScreenInfo();

  void set_src(const std::string &src) { src_ = src; }
  const std::string& src() const { return src_; }

 private:
  std::string src_;
};

class SplashScreenHandler : public ManifestHandler {
 public:
  SplashScreenHandler();
  virtual ~SplashScreenHandler();

  bool Parse(std::shared_ptr<ApplicationData> application,
             std::string* error) override;
  bool Validate(std::shared_ptr<const ApplicationData> application,
                std::string* error) const override;
  std::vector<std::string> Keys() const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(SplashScreenHandler);
};

}  // namespace widget_manifest_parser
}  // namespace common_installer

#endif  // WIDGET_MANIFEST_PARSER_MANIFEST_HANDLERS_SPLASH_SCREEN_HANDLER_H_
