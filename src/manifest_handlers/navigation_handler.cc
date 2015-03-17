// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "manifest_handlers/navigation_handler.h"

#include <boost/tokenizer.hpp>

#include "manifest_handlers/application_manifest_constants.h"

namespace wgt {
namespace parse {

namespace keys = wgt::application_widget_keys;

namespace {

const boost::char_separator<char> navigation_separator(" ");

}  // namespace

NavigationInfo::NavigationInfo(const std::string& allowed_domains) {
  boost::tokenizer<boost::char_separator<char>> tokens(
      allowed_domains, navigation_separator);
  for (auto& item : tokens) {
    allowed_domains_.push_back(item);
  }
}

NavigationInfo::~NavigationInfo() {
}

NavigationHandler::NavigationHandler() {
}

NavigationHandler::~NavigationHandler() {
}

bool NavigationHandler::Parse(
    const parser::Manifest& manifest,
    std::shared_ptr<parser::ManifestData>* output,
    std::string* error) {
  if (!manifest.HasPath(keys::kAllowNavigationKey)) {
    return true;
  }
  std::string allowed_domains;
  if (!manifest.GetString(keys::kAllowNavigationKey, &allowed_domains)) {
    *error = "Invalid value of allow-navigation.";
    return false;
  }
  if (allowed_domains.empty()) {
    *output = std::static_pointer_cast<parser::ManifestData>(
        std::make_shared<NavigationInfo>(""));
    return true;
  }

  *output = std::static_pointer_cast<parser::ManifestData>(
      std::make_shared<NavigationInfo>(allowed_domains));
  return true;
}

bool NavigationHandler::Validate(
    const parser::ManifestData& data,
    const parser::ManifestDataMap& /*handlers_output*/,
    std::string* /*error*/) const {
  const NavigationInfo& navi_info = static_cast<const NavigationInfo&>(data);
  // TODO(j.izydorczyk): There should be done 'navi_info' validity check.
  (void) navi_info;
  return true;
}

std::string NavigationHandler::Key() const {
  return keys::kAllowNavigationKey;
}

}  // namespace parse
}  // namespace wgt
