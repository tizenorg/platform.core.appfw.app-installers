// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "widget-manifest-parser/manifest_handlers/splash_screen_handler.h"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <cassert>
#include <map>
#include <utility>

#include "widget-manifest-parser/application_manifest_constants.h"

namespace bf = boost::filesystem;

namespace common_installer {
namespace widget_manifest_parser {

namespace keys = application_widget_keys;

SplashScreenInfo::SplashScreenInfo() {}
SplashScreenInfo::~SplashScreenInfo() {}

SplashScreenHandler::SplashScreenHandler() {}

SplashScreenHandler::~SplashScreenHandler() {}

bool SplashScreenHandler::Parse(std::shared_ptr<ApplicationData> application,
                                     std::string* /*error*/) {
  std::shared_ptr<SplashScreenInfo> ss_info(new SplashScreenInfo);
  const Manifest* manifest = application->GetManifest();
  assert(manifest);

  utils::Value* splash_screen = NULL;
  manifest->Get(keys::kTizenSplashScreenKey, &splash_screen);
  if (splash_screen && splash_screen->IsType(utils::Value::TYPE_DICTIONARY)) {
    utils::DictionaryValue* ss_dict = NULL;
    splash_screen->GetAsDictionary(&ss_dict);
    std::string src;
    ss_dict->GetString(keys::kTizenSplashScreenSrcKey, &src);
    ss_info->set_src(src);
  }
  application->SetManifestData(keys::kTizenSplashScreenKey, ss_info);
  return true;
}

bool SplashScreenHandler::Validate(
    std::shared_ptr<const ApplicationData> application,
    std::string* error) const {
  const Manifest* manifest = application->GetManifest();
  assert(manifest);
  utils::Value* splash_screen = NULL;
  manifest->Get(keys::kTizenSplashScreenKey, &splash_screen);
  if (!splash_screen || !splash_screen->IsType(utils::Value::TYPE_DICTIONARY)) {
    *error = "The splash-screen attribute is not set correctly.";
    return false;
  }
  utils::DictionaryValue* ss_dict = NULL;
  splash_screen->GetAsDictionary(&ss_dict);
  std::string ss_src;
  ss_dict->GetString(keys::kTizenSplashScreenSrcKey, &ss_src);
  bf::path path = application->path() / bf::path(ss_src);
  if (!bf::exists(path)) {
    *error = "The splash screen image does not exist";
    return false;
  }
  return true;
}

std::vector<std::string> SplashScreenHandler::Keys() const {
  return std::vector<std::string>(1, keys::kTizenSplashScreenKey);
}

}  // namespace widget_manifest_parser
}  // namespace common_installer
