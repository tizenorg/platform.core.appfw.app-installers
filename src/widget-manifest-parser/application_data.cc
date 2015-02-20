// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "widget-manifest-parser/application_data.h"

#include <cassert>
#include <regex>

#include "utils/logging.h"
#include "widget-manifest-parser/application_manifest_constants.h"
#include "widget-manifest-parser/manifest.h"
#include "widget-manifest-parser/manifest_handler.h"
#include "widget-manifest-parser/manifest_handlers/permissions_handler.h"
#include "widget-manifest-parser/manifest_handlers/widget_handler.h"
#include "widget-manifest-parser/manifest_handlers/tizen_application_handler.h"

namespace keys = common_installer::application_manifest_keys;
namespace widget_keys = common_installer::application_widget_keys;
namespace errors = common_installer::application_manifest_errors;

namespace {
const char kApplicationScheme[] = "app";
const char kWGTAppIdPattern[] = "\\A([0-9a-zA-Z]{10})[.][0-9a-zA-Z]{1,52}\\z";
const std::string kAppIdPrefix("xwalk.");

const size_t kIdSize = 16;

bool IsValidWGTID(const std::string& id) {
  std::regex regex(kWGTAppIdPattern);
  return std::regex_match (id, regex);
}

std::string AppIdToPkgId(const std::string& id) {
  std::string package_id;
  if (!IsValidWGTID(id)) {
    LOG(WARNING) << "Cannot get package_id from invalid app id";
    return std::string();
  }
  return package_id;
}

}  // namespace

namespace common_installer {
namespace widget_manifest_parser {

// static
std::shared_ptr<ApplicationData> ApplicationData::Create(
    const bf::path& path, const std::string& explicit_id,
    SourceType source_type, std::unique_ptr<Manifest> manifest,
    std::string* error_message) {
  assert(error_message);
  std::string error;
  if (!manifest->ValidateManifest(error_message))
    return nullptr;

  std::shared_ptr<ApplicationData> app_data(
      new ApplicationData(path, source_type, std::move(manifest)));
  if (!app_data->Init(explicit_id, &error)) {
    *error_message = error;
    return nullptr;
  }

  ManifestHandlerRegistry* registry =
      ManifestHandlerRegistry::GetInstance(app_data->manifest_type());

  if (!registry->ValidateAppManifest(app_data, error_message))
    return nullptr;
  return app_data;
}

// static
std::string ApplicationData::GetBaseURLFromApplicationId(
    const std::string& application_id) {
  return (std::string(kApplicationScheme) +
      "://" + application_id + "/");
}

ApplicationData::ManifestData* ApplicationData::GetManifestData(
        const std::string& key) const {
  assert(finished_parsing_manifest_);
  ManifestDataMap::const_iterator iter = manifest_data_.find(key);
  if (iter != manifest_data_.end())
    return iter->second.get();
  return nullptr;
}

void ApplicationData::SetManifestData(const std::string& key,
    std::shared_ptr<ApplicationData::ManifestData> data) {
  assert(!finished_parsing_manifest_);
  manifest_data_[key] = data;
}

std::string ApplicationData::GetPackageID() const {
  return AppIdToPkgId(application_id_);
}

const std::string ApplicationData::VersionString() const {
  if (!version_.empty())
    return Version();
  return "";
}

bool ApplicationData::IsHostedApp() const {
  bool hosted = source_type_ == EXTERNAL_URL;
  if (manifest_->HasPath(widget_keys::kContentNamespace)) {
    std::string ns;
    if (manifest_->GetString(widget_keys::kContentNamespace, &ns) &&
        ns == widget_keys::kTizenNamespacePrefix)
      hosted = true;
  }
  return hosted;
}

ApplicationData::ApplicationData(const bf::path& path,
  SourceType source_type, std::unique_ptr<Manifest> manifest)
    : manifest_version_(0),
      path_(path),
      manifest_(std::move(manifest)),
      finished_parsing_manifest_(false),
      source_type_(source_type) {
  assert(path_.empty() || path_.is_absolute());
}

ApplicationData::~ApplicationData() {
}

bool ApplicationData::Init(const std::string& explicit_id,
                           std::string* error) {
  assert(error);
  ManifestHandlerRegistry* registry =
      ManifestHandlerRegistry::GetInstance(manifest_type());
  if (!registry->ParseAppManifest(shared_from_this(), error))
    return false;

  if (!LoadID(explicit_id, error))
    return false;
  if (!LoadName(error))
    return false;
  if (!LoadVersion(error))
    return false;

  application_url_ = ApplicationData::GetBaseURLFromApplicationId(ID());
  finished_parsing_manifest_ = true;
  return true;
}

bool ApplicationData::LoadID(const std::string& explicit_id,
                             std::string* /*error*/) {
  std::string application_id;
  auto iter = manifest_data_.find(widget_keys::kTizenApplicationKey);
  if (iter == manifest_data_.end())
    return false;
  const TizenApplicationInfo* tizen_app_info =
      static_cast<TizenApplicationInfo*>(iter->second.get());
  assert(tizen_app_info);
  application_id = tizen_app_info->id();
  if (!application_id.empty()) {
    application_id_ = application_id;
    return true;
  }
  if (!explicit_id.empty()) {
    application_id_ = explicit_id;
    return true;
  }
  return false;
}

bool ApplicationData::LoadName(std::string* error) {
  assert(error);
  std::string localized_name;
  std::string name_key(GetNameKey(manifest_type()));

  if (!manifest_->GetString(name_key, &localized_name) &&
      manifest_type() == Manifest::TYPE_MANIFEST) {
    *error = errors::kInvalidName;
    return false;
  }
  non_localized_name_ = localized_name;

  // TODO(jizydorczyk): This needs to be implemented
  // utils::AdjustStringForLocaleDirection(&localized_name);
  name_ = localized_name;
  return true;
}

bool ApplicationData::LoadVersion(std::string* error) {
  assert(error);
  std::string version_str;

  version_ = "";

  bool ok = manifest_->GetString(widget_keys::kVersionKey, &version_str);
  if (!ok) {
    *error = errors::kInvalidVersion;
    return true;
  }

  version_ = std::string(version_str);
  return true;
}

bool ApplicationData::SetApplicationLocale(const std::string& locale,
                                           std::string* error) {
  manifest_->SetSystemLocale(locale);
  if (!LoadName(error))
    return false;

  if (WidgetInfo* widget_info = static_cast<WidgetInfo*>(
          GetManifestData(widget_keys::kWidgetKey))) {
    std::string string_value;
    if (manifest_->GetString(widget_keys::kNameKey, &string_value))
      widget_info->SetName(string_value);
    if (manifest_->GetString(widget_keys::kShortNameKey, &string_value))
      widget_info->SetShortName(string_value);
    if (manifest_->GetString(widget_keys::kDescriptionKey, &string_value))
      widget_info->SetDescription(string_value);
  }
  return true;
}

}   // namespace widget_manifest_parser
}   // namespace common_installer
