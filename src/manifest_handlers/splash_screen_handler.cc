// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "manifest_handlers/splash_screen_handler.h"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <cassert>
#include <map>
#include <utility>

#include "manifest_handlers/application_manifest_constants.h"

namespace bf = boost::filesystem;

namespace wgt {
namespace parse {

namespace keys = wgt::application_widget_keys;

SplashScreenInfo::SplashScreenInfo() {}
SplashScreenInfo::~SplashScreenInfo() {}

SplashScreenHandler::SplashScreenHandler() {}

SplashScreenHandler::~SplashScreenHandler() {}

bool SplashScreenHandler::Parse(
    const parser::Manifest& manifest,
    std::shared_ptr<parser::ManifestData>* output,
    std::string* error) {
  std::shared_ptr<SplashScreenInfo> ss_info(new SplashScreenInfo);
  parser::utils::Value* splash_screen = nullptr;
  manifest.Get(keys::kTizenSplashScreenKey, &splash_screen);
  if (splash_screen && splash_screen->IsType(
          parser::utils::Value::TYPE_DICTIONARY)) {
    parser::utils::DictionaryValue* ss_dict = nullptr;
    splash_screen->GetAsDictionary(&ss_dict);
    std::string src;
    ss_dict->GetString(keys::kTizenSplashScreenSrcKey, &src);
    ss_info->set_src(src);
  } else if (splash_screen && !splash_screen->IsType(
                 parser::utils::Value::TYPE_DICTIONARY)) {
    *error = "splash-screen elements type is not TYPE_DICTIONARY";
    return false;
  } else {
    ss_info->set_exists(false);
  }
  *output = std::static_pointer_cast<parser::ManifestData>(ss_info);
  return true;
}

bool SplashScreenHandler::Validate(
    const parser::ManifestData& data,
    const parser::ManifestDataMap& /*handlers_output*/,
    std::string* error) const {
  const SplashScreenInfo& splash_data =
       static_cast<const SplashScreenInfo&>(data);
  std::string src = splash_data.src();
  // According to w3c specification splash screen image should be of one of
  // below types.
  if (src.compare(src.size() - 3, 3, "png") &&
      src.compare(src.size() - 3, 3, "svg") &&
      src.compare(src.size() - 3, 3, "gif") &&
      src.compare(src.size() - 3, 3, "jpg")) {
    *error = "Not supported file extension of splash image";
    return false;
  }
  return true;
}

std::string SplashScreenHandler::Key() const {
  return keys::kTizenSplashScreenKey;
}

}  // namespace parse
}  // namespace wgt
