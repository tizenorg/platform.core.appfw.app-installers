// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "widget-manifest-parser/manifest_handlers/app_control_handler.h"

#include <iri.h>

#include "utils/values.h"
#include "widget-manifest-parser/app_control_info.h"
#include "widget-manifest-parser/application_manifest_constants.h"

namespace common_installer {
namespace widget_manifest_parser {

namespace keys = application_widget_keys;

namespace {

void ParseAppControlEntryAndStore(const utils::DictionaryValue& control_dict,
    AppControlInfoList* aplist) {
  std::string src;
  const utils::DictionaryValue* src_dict;
  if (control_dict.GetDictionary(keys::kTizenApplicationAppControlSrcKey,
      &src_dict)) {
    src_dict->GetString(
        keys::kTizenApplicationAppControlChildNameAttrKey, &src);
  }

  std::string operation;
  const utils::DictionaryValue* operation_dict;
  if (control_dict.GetDictionary(
      keys::kTizenApplicationAppControlOperationKey,
      &operation_dict)) {
    operation_dict->GetString(
        keys::kTizenApplicationAppControlChildNameAttrKey, &operation);
  }

  std::string uri;
  const utils::DictionaryValue* uri_dict;
  if (control_dict.GetDictionary(keys::kTizenApplicationAppControlUriKey,
      &uri_dict)) {
    uri_dict->GetString(
        keys::kTizenApplicationAppControlChildNameAttrKey, &uri);
  }

  std::string mime;
  const utils::DictionaryValue* mime_dict;
  if (control_dict.GetDictionary(keys::kTizenApplicationAppControlMimeKey,
      &mime_dict)) {
    mime_dict->GetString(
        keys::kTizenApplicationAppControlChildNameAttrKey, &mime);
  }

  aplist->controls.emplace_back(src, operation, uri, mime);
}

}  // namespace

AppControlHandler::AppControlHandler() {
}

AppControlHandler::~AppControlHandler() {
}

bool AppControlHandler::Parse(std::shared_ptr<ApplicationData> application,
    std::string* error) {
  const Manifest* manifest = application->GetManifest();
  std::shared_ptr<AppControlInfoList> aplist(new AppControlInfoList());
  utils::Value* value;
  manifest->Get(keys::kTizenApplicationAppControlsKey, &value);

  if (value && value->GetType() == utils::Value::TYPE_LIST) {
    // multiple entries
    const utils::ListValue* list;
    value->GetAsList(&list);
    for (const auto& item : *list) {
      const utils::DictionaryValue* control_dict;
      if (!item->GetAsDictionary(&control_dict)) {
        *error = "Parsing app-control element failed";
        return false;
      }

      ParseAppControlEntryAndStore(*control_dict, aplist.get());
    }
  } else if (value->GetType() == utils::Value::TYPE_DICTIONARY) {
    // single entry
    const utils::DictionaryValue* dict;
    value->GetAsDictionary(&dict);
    ParseAppControlEntryAndStore(*dict, aplist.get());
  } else {
    *error = "Cannot parse app-control element";
    return false;
  }

  application->SetManifestData(
      keys::kTizenApplicationAppControlsKey,
      aplist);
  return true;
}

bool AppControlHandler::Validate(
    std::shared_ptr<const ApplicationData> application,
    std::string* error) const {
  const AppControlInfoList* app_controls =
      static_cast<const AppControlInfoList*>(
          application->GetManifestData(keys::kTizenApplicationAppControlsKey));

  for (const auto& item : app_controls->controls) {
    if (item.src().empty()) {
      *error = "The src child element of app-control element is obligatory";
      return false;
    }

    const std::string& operation = item.operation();
    if (operation.empty()) {
      *error =
          "The operation child element of app-control element is obligatory";
      return false;
    }
    std::unique_ptr<iri_struct, decltype(&iri_destroy)> iri(
        iri_parse(operation.c_str()), iri_destroy);
    if (!iri) {
      *error = "Libiri failed";
      return false;
    }
    if (!iri->scheme || !iri->host) {
      *error =
          "The operation child element of app-control element is not valid url";
      return false;
    }
  }
  return true;
}

std::vector<std::string> AppControlHandler::Keys() const {
  return std::vector<std::string>(1, keys::kTizenApplicationAppControlsKey);
}

}   // namespace widget_manifest_parser
}   // namespace common_installer
