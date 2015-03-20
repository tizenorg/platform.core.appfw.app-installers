// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "parser/manifest_handlers/tizen_application_handler.h"

#include <cassert>
#include <map>
#include <utility>

#include "utils/logging.h"
#include "utils/values.h"
#include "parser/application_manifest_constants.h"
#include "parser/manifest_util.h"

namespace {
const char kTizenWebAPIVersion[] = "2.2";
}

namespace common_installer {

namespace keys = application_widget_keys;

namespace parser {

TizenApplicationInfo::TizenApplicationInfo() {
}

TizenApplicationInfo::~TizenApplicationInfo() {
}

TizenApplicationHandler::TizenApplicationHandler() {}

TizenApplicationHandler::~TizenApplicationHandler() {}

bool TizenApplicationHandler::Parse(
    std::shared_ptr<ApplicationData> application, std::string* error) {
  std::shared_ptr<TizenApplicationInfo> app_info(new TizenApplicationInfo);
  const Manifest* manifest = application->GetManifest();
  assert(manifest);

  utils::Value* app_value = nullptr;
  manifest->Get(keys::kTizenApplicationKey, &app_value);
  // Find an application element with tizen namespace
  utils::DictionaryValue* app_dict;
  std::string value;
  bool find = false;
  if (app_value && app_value->IsType(utils::Value::TYPE_DICTIONARY)) {
    app_value->GetAsDictionary(&app_dict);
    find = app_dict->GetString(keys::kNamespaceKey, &value);
    find = find && (value == keys::kTizenNamespacePrefix);
  } else if (app_value && app_value->IsType(utils::Value::TYPE_LIST)) {
    utils::ListValue* list;
    app_value->GetAsList(&list);
    for (utils::ListValue::iterator it = list->begin();
         it != list->end(); ++it) {
      (*it)->GetAsDictionary(&app_dict);
      find = app_dict->GetString(keys::kNamespaceKey, &value);
      find = find && (value == keys::kTizenNamespacePrefix);
      if (find)
        break;
    }
  }

  if (!find) {
    *error = "Cannot find application element with tizen namespace "
             "or the tizen namespace prefix is incorrect.\n";
    return false;
  }
  if (app_dict->GetString(keys::kTizenApplicationIdKey, &value))
    app_info->set_id(value);
  if (app_dict->GetString(keys::kTizenApplicationPackageKey, &value)) {
    app_info->set_package(value);
  }
  if (app_dict->GetString(keys::kTizenApplicationRequiredVersionKey, &value))
    app_info->set_required_version(value);

  application->SetManifestData(keys::kTizenApplicationKey,
                               app_info);
  return true;
}

bool TizenApplicationHandler::Validate(
    std::shared_ptr<const ApplicationData> application,
    std::string* error) const {
  const TizenApplicationInfo* app_info =
      static_cast<const TizenApplicationInfo*>(
          application->GetManifestData(keys::kTizenApplicationKey));

  if (!ValidateTizenApplicationId(app_info->id())) {
    *error = "The id property of application element "
             "does not match the format\n";
    return false;
  }

  if (!ValidateTizenPackageId(app_info->package())) {
    *error = "The package property of application element "
             "does not match the format\n";
    return false;
  }

  if (app_info->id().find(app_info->package()) != 0) {
    *error = "The application element property id "
             "does not start with package.\n";
    LOG(ERROR) << "app_info->id() = " << app_info->id();
    LOG(ERROR) << "app_info->package() = " << app_info->package();
    return false;
  }
  if (app_info->required_version().empty()) {
    *error = "The required_version property of application "
             "element does not exist.\n";
    return false;
  }

  const std::string supported_version = kTizenWebAPIVersion;
  if (supported_version.compare(app_info->required_version()) < 0) {
    *error = "The required_version of Tizen Web API "
             "is not supported.\n";
    return false;
  }

  return true;
}

std::vector<std::string> TizenApplicationHandler::Keys() const {
  return std::vector<std::string>(1, keys::kTizenApplicationKey);
}

}  // namespace parser
}  // namespace common_installer
