// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "manifest_handlers/application_icons_handler.h"

#include <string>

#include "utils/logging.h"

namespace wgt {
namespace parse {

namespace {
  const char kWidgetIconFullKey[] = "widget.icon";
  const char kWidgetIconSrcKey[] = "@src";
}

std::vector<std::string> ApplicationIconsInfo::get_icon_paths() const {
  return icon_paths_;
}

void ApplicationIconsInfo::add_icon_path(std::string icon_path) {
  icon_paths_.push_back(icon_path);
}

ApplicationIconsHandler::ApplicationIconsHandler() {
}

ApplicationIconsHandler::~ApplicationIconsHandler() {
}

bool ApplicationIconsHandler::ExtractIconSrc(
    const parser::utils::Value& dict, std::string* value,
    std::string* error) {
  const parser::utils::DictionaryValue* inner_dict;
  if (!dict.GetAsDictionary(&inner_dict)) {
    LOG(INFO) << "Cannot get key value as a dictionary. Key name: widget.icon";
    return true;
  }
  std::string src;
  if (!inner_dict->GetString(kWidgetIconSrcKey, &src)) {
    LOG(INFO) << "Cannot find mandatory key. Key name: .@src";
    return true;
  }
  *value = src;
  return  true;
}

bool ApplicationIconsHandler::Parse(
    const parser::Manifest& manifest,
    std::shared_ptr<parser::ManifestData>* output,
    std::string* error) {
  std::shared_ptr<ApplicationIconsInfo> app_icons_info =
      std::make_shared<ApplicationIconsInfo>();
  parser::utils::Value* key_value;
  if (!manifest.Get(kWidgetIconFullKey, &key_value)) {
    return true;
  }

  if (key_value->IsType(parser::utils::Value::TYPE_DICTIONARY)) {
    std::string icon;
    if (!ExtractIconSrc(*key_value, &icon, error)) {
      *error = "Cannot get key value as a dictionary. Key name: widget.icon";
      return false;
    }
    app_icons_info->add_icon_path(icon);
  } else if (key_value->IsType(parser::utils::Value::TYPE_LIST)) {
    const parser::utils::ListValue* list;
    if (!key_value->GetAsList(&list)) {
      *error = "Cannot get key value as a list. Key name: widget.icon";
      return false;
    }
    for (const parser::utils::Value* list_value : *list) {
      std::string icon;
      if (!ExtractIconSrc(*list_value, &icon, error)) {
        *output =
            std::static_pointer_cast<parser::ManifestData>(app_icons_info);
        return true;
      }
      app_icons_info->add_icon_path(icon);
    }
  }
  *output = std::static_pointer_cast<parser::ManifestData>(app_icons_info);
  return true;
}

bool ApplicationIconsHandler::Validate(
    const parser::ManifestData& data,
    const parser::ManifestDataMap& /*handlers_output*/,
    std::string* error) const {
  const ApplicationIconsInfo& icon_info =
      static_cast<const ApplicationIconsInfo&>(data);
  std::vector<std::string> icon_paths = icon_info.get_icon_paths();
  for (const auto& src : icon_paths) {
    if (!src.empty() && src.compare(src.size() - 3, 3, "png") &&
        src.compare(src.size() - 3, 3, "svg") &&
        src.compare(src.size() - 3, 3, "gif") &&
        src.compare(src.size() - 3, 3, "jpg") &&
        src.compare(src.size() - 3, 3, "ico")) {
      *error = "Image extension not supported";
      return false;
    }
  }
  return true;
}

bool ApplicationIconsHandler::AlwaysParseForType() const {
  return true;
}

std::string ApplicationIconsHandler::Key() const {
  return wgt::application_manifest_keys::kIconsKey;
}

}  // namespace parse
}  // namespace wgt
