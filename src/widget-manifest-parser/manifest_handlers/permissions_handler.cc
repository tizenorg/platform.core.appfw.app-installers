// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "widget-manifest-parser/manifest_handlers/permissions_handler.h"

#include "utils/logging.h"
#include "utils/values.h"
#include "widget-manifest-parser/application_manifest_constants.h"

namespace common_installer {

namespace keys = application_widget_keys;

namespace widget_manifest_parser {

PermissionsInfo::PermissionsInfo() {
}

PermissionsInfo::~PermissionsInfo() {
}

PermissionsHandler::PermissionsHandler() {
}

PermissionsHandler::~PermissionsHandler() {
}

bool PermissionsHandler::Parse(
    std::shared_ptr<ApplicationData> application, std::u16string* error) {
  std::shared_ptr<PermissionsInfo> permissions_info(new PermissionsInfo);
  if (!application->GetManifest()->HasPath(keys::kTizenPermissionsKey)) {
    application->SetManifestData(
        keys::kTizenPermissionsKey, permissions_info);
    return true;
  }

  utils::Value* value;
  if (!application->GetManifest()->Get(keys::kTizenPermissionsKey, &value)) {
    *error = u"Invalid value of tizen permissions.";
    return false;
  }

  std::unique_ptr<utils::ListValue> permission_list;
  if (value->IsType(utils::Value::TYPE_DICTIONARY)) {
    permission_list.reset(new utils::ListValue);
    permission_list->Append(value->DeepCopy());
  } else {
    utils::ListValue* list = nullptr;
    value->GetAsList(&list);
    if (list)
      permission_list.reset(list->DeepCopy());
  }

  if (!permission_list) {
    *error = u"Invalid value of permissions.";
    return false;
  }
  PermissionSet api_permissions;
  for (utils::ListValue::const_iterator it = permission_list->begin();
       it != permission_list->end(); ++it) {
    utils::DictionaryValue* dictionary_value = nullptr;
    (*it)->GetAsDictionary(&dictionary_value);

    std::string permission;
    if (!dictionary_value ||
        !dictionary_value->GetString(
            keys::kTizenPermissionsNameKey, &permission) ||
        permission.empty())
      continue;

    if (api_permissions.find(permission) != api_permissions.end())
      LOG(WARNING) << "Duplicated permission names found.";

    api_permissions.insert(permission);
  }

  permissions_info->SetAPIPermissions(api_permissions);
  application->SetManifestData(keys::kTizenPermissionsKey,
                               permissions_info);

  return true;
}

bool PermissionsHandler::AlwaysParseForType(Manifest::Type type) const {
  return type == Manifest::TYPE_WIDGET;
}

std::vector<std::string> PermissionsHandler::Keys() const {
  return std::vector<std::string>(1, keys::kTizenPermissionsKey);
}

}  // namespace widget_manifest_parser
}  // namespace common_installer
