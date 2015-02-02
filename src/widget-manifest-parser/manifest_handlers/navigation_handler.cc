// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.
// Part of this file is redistributed from crosswalk project.
// Check license file: LICENSE-xwalk

#include "widget-manifest-parser/manifest_handlers/navigation_handler.h"

#include <boost/tokenizer.hpp>

#include "widget-manifest-parser/application_manifest_constants.h"

namespace common_installer {
namespace widget_manifest_parser {

namespace keys = application_widget_keys;

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
    std::shared_ptr<ApplicationData> application_data,
    std::string* error) {
  if (!application_data->GetManifest()->HasPath(keys::kAllowNavigationKey))
    return true;
  std::string allowed_domains;
  if (!application_data->GetManifest()->GetString(keys::kAllowNavigationKey,
                                                  &allowed_domains)) {
    *error = "Invalid value of allow-navigation.";
    return false;
  }
  if (allowed_domains.empty())
    return true;

  application_data->SetManifestData(keys::kAllowNavigationKey,
      std::shared_ptr<ApplicationData::ManifestData>(
          new NavigationInfo(allowed_domains)));

  return true;
}

std::vector<std::string> NavigationHandler::Keys() const {
  return std::vector<std::string>(1, keys::kAllowNavigationKey);
}

}  // namespace widget_manifest_parser
}  // namespace common_installer
