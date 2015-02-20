// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "widget-manifest-parser/widget_manifest_parser.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <algorithm>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "utils/macros.h"
#include "utils/values.h"

#include "widget-manifest-parser/application_data.h"
#include "widget-manifest-parser/application_manifest_constants.h"
#include "widget-manifest-parser/manifest_util.h"
#include "widget-manifest-parser/manifest_handlers/permissions_handler.h"
#include "widget-manifest-parser/manifest_handlers/tizen_application_handler.h"
#include "widget-manifest-parser/manifest_handlers/widget_handler.h"
#include "widget-manifest-parser/permission_types.h"

namespace bf = boost::filesystem;
namespace keys = common_installer::application_widget_keys;
namespace parser = common_installer::widget_manifest_parser;

namespace {

const char kErrMsgNoPath[] =
    "Path not specified.";
const char kErrMsgInvalidPath[] =
    "Invalid path.";
const char kErrMsgValueNotFound[] =
    "Value not found. Value name: ";
const char kErrMsgInvalidDictionary[] =
    "Cannot get key value as a dictionary. Key name: ";
const char kErrMsgInvalidList[] =
    "Cannot get key value as a list. Key name: ";
const char kErrMsgNoMandatoryKey[] =
    "Cannot find mandatory key. Key name: ";

const char kName[] = "name";
const char kShortName[] = "shortName";
const char kVersion[] = "version";
const char kWidgetIconFullKey[] = "widget.icon";
const char kWidgetIconSrcKey[] = "@src";

typedef std::string LocalError;

void SetError(const std::string& message, std::string* error) {
  *error = message;
}

void SetError(const std::string& message, const std::string& arg,
    std::string* error) {
  *error = message + arg;
}

}  // namespace

namespace common_installer {
namespace widget_manifest_parser {

WidgetManifestParser::WidgetManifestParser() :
  valid_(false) {
}

const std::string& WidgetManifestParser::GetErrorMessage() {
  return error_;
}

bool WidgetManifestParser::FillManifestX(manifest_x* manifest) {
  if (!valid_)
    return false;
  std::shared_ptr<parser::ApplicationData> app_data =
      parser::ApplicationData::Create(bf::path(), std::string(),
                                      parser::ApplicationData::INTERNAL,
                                      std::move(manifest_), &error_);
  if (!app_data.get())
    return false;
  parser::TizenApplicationInfo* app_info;
  if (!(app_info = ExtractApplicationInfo(*app_data))) {
    SetError(kErrMsgValueNotFound, "application info", &error_);
    return false;
  }
  api_version_ = app_info->required_version();
  parser::WidgetInfo* wgt_info;
  if (!(wgt_info = ExtractWidgetInfo(*app_data))) {
    SetError(kErrMsgValueNotFound, "widget info", &error_);
    return false;
  }
  wgt_info->GetWidgetInfo()->GetString(kShortName, &short_name_);

  std::vector<std::string> icons = ExtractIcons(*app_data->GetManifest(),
                                                &error_);
  if (!error_.empty())
    return false;
  std::set<std::string> privileges = ExtractPrivileges(*app_data, &error_);
  if (!error_.empty())
    return false;
  // application data
  manifest->serviceapplication = nullptr;
  manifest->uiapplication = reinterpret_cast<uiapplication_x*>
    (calloc (1, sizeof(uiapplication_x)));
  manifest->uiapplication->label = reinterpret_cast<label_x*>
    (calloc(1, sizeof(label_x)));
  manifest->uiapplication->appcontrol = nullptr;

  manifest->uiapplication->appid = strdup(app_info->id().c_str());
  manifest->uiapplication->type = strdup("webapp");

  // package data
  manifest->package = strdup(app_info->package().c_str());
  manifest->mainapp_id = strdup(app_info->id().c_str());

  std::string version;
  wgt_info->GetWidgetInfo()->GetString(kVersion, &version);
  manifest->version = strdup(version.c_str());
  manifest->type = strdup("wgt");

  description_x* description = reinterpret_cast<description_x*>
      (calloc(1, sizeof(description_x)));
  std::string name;
  wgt_info->GetWidgetInfo()->GetString(kName, &name);
  description->name = strdup(name.c_str());
  manifest->description = description;
  manifest->label =
      reinterpret_cast<label_x*>(calloc(1, sizeof(label_x)));
  manifest->label->name = strdup(name.c_str());

  // icons
  for (std::string icon_str : icons) {
    icon_x* icon = reinterpret_cast<icon_x*> (calloc(1, sizeof(icon_x)));
    icon->name = strdup(icon_str.c_str());
    LISTADD(manifest->icon, icon);
  }

  // privileges
  privileges_x* privileges_x_list =
      reinterpret_cast<privileges_x*> (calloc(1, sizeof(privileges_x)));\
  manifest->privileges = privileges_x_list;
  for (const std::string& p : privileges) {
    privilege_x* privilege_x_node =
        reinterpret_cast<privilege_x*> (calloc(1, sizeof(privilege_x)));
    privilege_x_node->text = strdup(p.c_str());
    LISTADD(manifest->privileges->privilege, privilege_x_node);
  }

  // For wgt package use the long name if the short name is empty
  if (!short_name_.empty())
    manifest->uiapplication->label->name = strdup(short_name_.c_str());
  else
    manifest->uiapplication->label->name =
        strdup(manifest->description->name);

  manifest->uiapplication->icon =
      reinterpret_cast<icon_x*> (calloc(1, sizeof(icon_x)));
  manifest->uiapplication->icon->name = strdup(manifest->icon->name);
  manifest->uiapplication->next = nullptr;
  return true;
}

WidgetManifestParser::~WidgetManifestParser() {
}

parser::TizenApplicationInfo* WidgetManifestParser::ExtractApplicationInfo(
    const parser::ApplicationData& app_data) {
  parser::TizenApplicationInfo* info =
      static_cast<parser::TizenApplicationInfo*>(
          app_data.GetManifestData(keys::kTizenApplicationKey));
  return info;
}

parser::WidgetInfo* WidgetManifestParser::ExtractWidgetInfo(const ApplicationData& app_data) {
  parser::WidgetInfo* info = static_cast<parser::WidgetInfo*>(
      app_data.GetManifestData(keys::kWidgetKey));
  return info;
}

bool WidgetManifestParser::ExtractIconSrc(
    const common_installer::utils::Value& dict, std::string* value,
    std::string* error) {
  const common_installer::utils::DictionaryValue* inner_dict;
  if (!dict.GetAsDictionary(&inner_dict)) {
    SetError(kErrMsgInvalidDictionary, kWidgetIconFullKey, error);
    return false;
  }
  std::string src;
  if (!inner_dict->GetString(kWidgetIconSrcKey, &src)) {
    SetError(kErrMsgNoMandatoryKey, kWidgetIconSrcKey, error);
    return false;
  }
  *value = src;
  return  true;
}

std::vector<std::string> WidgetManifestParser::ExtractIcons(
    const parser::Manifest& manifest,
    std::string* error) {
  common_installer::utils::Value* key_value;
  std::vector<std::string> icons;
  if (!manifest.Get(kWidgetIconFullKey, &key_value)) {
    icons.clear();
    return icons;  // no icon, no error
  }

  if (key_value->IsType(common_installer::utils::Value::TYPE_DICTIONARY)) {
    std::string icon;
    if (!ExtractIconSrc(*key_value, &icon, error))
      return icons;
    icons.push_back(icon);
  } else if (key_value->IsType(common_installer::utils::Value::TYPE_LIST)) {
    const common_installer::utils::ListValue* list;
    if (!key_value->GetAsList(&list)) {
      SetError(kErrMsgInvalidList, kWidgetIconFullKey, error);
      return icons;
    }
    for (const common_installer::utils::Value* list_value : *list) {
      std::string icon;
      if (!ExtractIconSrc(*list_value, &icon, error))
        return icons;
      icons.push_back(icon);
    }
  }
  return icons;
}

std::set<std::string> WidgetManifestParser::ExtractPrivileges(
    const parser::ApplicationData& app_data, std::string* /*error*/) {
  const parser::PermissionsInfo* perm_info =
       static_cast<parser::PermissionsInfo*>(
            app_data.GetManifestData(keys::kTizenPermissionsKey));
  parser::PermissionSet permissions = perm_info->GetAPIPermissions();
  return permissions;
}

bool WidgetManifestParser::ParseManifest(const bf::path& manifest_path) {
  error_.clear();
  if (manifest_path.empty()) {
    SetError(kErrMsgNoPath, &error_);
    return false;
  }
  if (!exists(manifest_path)) {
    SetError(kErrMsgInvalidPath, &error_);
    return false;
  }

  manifest_ = parser::LoadManifest(
      manifest_path.string(), parser::Manifest::TYPE_WIDGET, &error_);
  if (!manifest_)
    return false;
  valid_ = error_.empty();
  return valid_;
}

const std::string& WidgetManifestParser::GetShortName() const {
  return short_name_;
}

const std::string& WidgetManifestParser::GetRequiredAPIVersion() const {
  return api_version_;
}

}  // namespace widget_manifest_parser
}  // namespace common_installer
