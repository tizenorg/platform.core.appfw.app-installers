// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "widget-manifest-parser/widget_manifest_parser.h"

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

#define API_EXPORT __attribute__((visibility("default")))

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

void SetError(const std::string& message, const char** error) {
  if (error)
    *error = strdup(message.c_str());
}

void SetError(const std::string& message, const std::string& arg,
    const char** error) {
  if (error)
    *error = strdup((message + arg).c_str());
}

bool ExtractPackage(
    const parser::ApplicationData& app_data, std::string* value) {
  parser::TizenApplicationInfo* info =
      static_cast<parser::TizenApplicationInfo*>(
          app_data.GetManifestData(keys::kTizenApplicationKey));
  if (info)
    *value = info->package();
  return info;
}

bool ExtractId(
    const parser::ApplicationData& app_data, std::string* value) {
  parser::TizenApplicationInfo* info =
      static_cast<parser::TizenApplicationInfo*>(
          app_data.GetManifestData(keys::kTizenApplicationKey));
  if (info)
    *value = info->id();
  return info;
}

bool ExtractName(const parser::ApplicationData& app_data, std::string* value) {
  parser::WidgetInfo* info = static_cast<parser::WidgetInfo*>(
      app_data.GetManifestData(keys::kWidgetKey));
  if (!info)
    return false;
  return info->GetWidgetInfo()->GetString(kName, value);
}

bool ExtractShortName(const parser::ApplicationData& app_data,
                      std::string* value) {
  parser::WidgetInfo* info = static_cast<parser::WidgetInfo*>(
      app_data.GetManifestData(keys::kWidgetKey));
  if (!info)
    return false;
  return info->GetWidgetInfo()->GetString(kShortName, value);
}

bool ExtractVersion(const parser::ApplicationData& app_data,
                    std::string* value) {
  parser::WidgetInfo* info = static_cast<parser::WidgetInfo*>(
      app_data.GetManifestData(keys::kWidgetKey));
  if (!info)
    return false;
  return info->GetWidgetInfo()->GetString(kVersion, value);
}

bool ExtractIconSrc(const common_installer::utils::Value& dict,
    std::string* value, const char** error) {
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

bool ExtractIcons(const parser::Manifest& manifest,
                  std::vector<std::string>* value, const char** error) {
  common_installer::utils::Value* key_value;
  if (!manifest.Get(kWidgetIconFullKey, &key_value)) {
    value->clear();
    return true;  // no icon, no error
  }

  std::vector<std::string> icons;
  if (key_value->IsType(common_installer::utils::Value::TYPE_DICTIONARY)) {
    std::string icon;
    if (!ExtractIconSrc(*key_value, &icon, error))
      return false;
    icons.push_back(icon);
  } else if (key_value->IsType(common_installer::utils::Value::TYPE_LIST)) {
    const common_installer::utils::ListValue* list;
    if (!key_value->GetAsList(&list)) {
      SetError(kErrMsgInvalidList, kWidgetIconFullKey, error);
      return false;
    }
    for (const common_installer::utils::Value* list_value : *list) {
      std::string icon;
      if (!ExtractIconSrc(*list_value, &icon, error))
        return false;
      icons.push_back(icon);
    }
  }

  value->swap(icons);
  return true;
}

bool ExtractApiVersion(const parser::ApplicationData& app_data,
                       std::string* value) {
  parser::TizenApplicationInfo* info =
      static_cast<parser::TizenApplicationInfo*>(
        app_data.GetManifestData(keys::kTizenApplicationKey));
  if (!info)
    return false;
  *value = info->required_version();
  return true;
}

bool ExtractPrivileges(const parser::ApplicationData& app_data,
                       std::set<std::string>* value, const char** error) {
  const parser::PermissionsInfo* perm_info =
       static_cast<parser::PermissionsInfo*>(
            app_data.GetManifestData(keys::kTizenPermissionsKey));
  parser::PermissionSet permissions = perm_info->GetAPIPermissions();
  *value = permissions;
  return true;
}

}  // namespace

extern "C" {

API_EXPORT bool ParseManifest(const char* path,
    const ManifestData** data, const char** error) {
  if (!path) {
    SetError(kErrMsgNoPath, error);
    return false;
  }
  std::string str_path = path;
  if (str_path.empty()) {
    SetError(kErrMsgInvalidPath, error);
    return false;
  }

  std::string local_error;

  std::unique_ptr<parser::Manifest> manifest = parser::LoadManifest(
      path, parser::Manifest::TYPE_WIDGET, &local_error);
  if (!manifest) {
    SetError(local_error, error);
    return false;
  }

  std::shared_ptr<parser::ApplicationData> app_data =
      parser::ApplicationData::Create(bf::path(), std::string(),
                                      parser::ApplicationData::INTERNAL,
                                      std::move(manifest), &local_error);
  if (!app_data.get()) {
    SetError(local_error, error);
    return false;
  }

  std::string package;
  if (!ExtractPackage(*app_data, &package)) {
    SetError(kErrMsgValueNotFound, "package id", error);
    return false;
  }

  std::string id;
  if (!ExtractId(*app_data, &id)) {
    SetError(kErrMsgValueNotFound, "application id", error);
    return false;
  }

  std::string name;
  if (!ExtractName(*app_data, &name)) {
    SetError(kErrMsgValueNotFound, "application name", error);
    return false;
  }

  std::string short_name;
  if (!ExtractShortName(*app_data, &short_name)) {
    SetError(kErrMsgValueNotFound, "application short name", error);
    return false;
  }

  std::string version;
  if (!ExtractVersion(*app_data, &version)) {
    SetError(kErrMsgValueNotFound, "application version", error);
    return false;
  }

  std::vector<std::string> icons;
  if (!ExtractIcons(*app_data->GetManifest(), &icons, error))
    return false;

  std::string api_version;
  if (!ExtractApiVersion(*app_data, &api_version)) {
    SetError(kErrMsgValueNotFound, "required api version", error);
    return false;
  }

  std::set<std::string> privileges;
  if (!ExtractPrivileges(*app_data, &privileges, error))
    return false;

  if (data) {
    ManifestData* manifest_data = new ManifestData;
    manifest_data->package = strdup(package.c_str());
    manifest_data->id = strdup(id.c_str());
    manifest_data->name = strdup(name.c_str());
    manifest_data->short_name = strdup(short_name.c_str());
    manifest_data->version = strdup(version.c_str());
    manifest_data->icon =
        icons.empty() ? strdup("") : strdup(icons.front().c_str());
    manifest_data->api_version = strdup(api_version.c_str());
    manifest_data->privilege_count = privileges.size();
    manifest_data->privilege_list = new char*[privileges.size()];
    char** privileges_it = manifest_data->privilege_list;
    for (const std::string& p : privileges) {
      *privileges_it = strdup(p.c_str());
      ++privileges_it;
    }
    *data = manifest_data;
  }

  return true;
}

API_EXPORT void ReleaseData(const ManifestData* data, const char* error) {
  free(data->package);
  free(data->id);
  free(data->name);
  free(data->short_name);
  free(data->version);
  free(data->icon);
  free(data->api_version);
  for (int i = 0; i < data->privilege_count; ++i)
    free(data->privilege_list[i]);
  delete[] data->privilege_list;
  delete data;
  free((void*)error);
}

}  // extern "C"
