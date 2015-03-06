// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "widget-manifest-parser/manifest_handlers/category_handler.h"

#include "utils/logging.h"
#include "utils/values.h"
#include "widget-manifest-parser/application_manifest_constants.h"

namespace common_installer {
namespace widget_manifest_parser {

namespace keys = application_widget_keys;

namespace {

const char kErrMsgCategory[] =
    "Parsing category element failed";
const char kErrMsgCategoryName[] =
    "The name element inside category element is obligatory";

bool ParseCategoryEntryAndStore(const utils::DictionaryValue& control_dict,
    CategoryInfoList* aplist) {
  std::string name;
  if (!control_dict.GetString(keys::kTizenCategoryNameKey, &name))
    return false;
  aplist->categories.push_back(name);
  return true;
}

}  // namespace

CategoryHandler::CategoryHandler() {
}

CategoryHandler::~CategoryHandler() {
}

bool CategoryHandler::Parse(std::shared_ptr<ApplicationData> application,
    std::string* error) {
  const Manifest* manifest = application->GetManifest();
  std::shared_ptr<CategoryInfoList> aplist(new CategoryInfoList());
  utils::Value* value;
  manifest->Get(keys::kTizenCategoryKey, &value);

  if (value->GetType() == utils::Value::TYPE_LIST) {
    // multiple entries
    const utils::ListValue* list;
    value->GetAsList(&list);
    for (const auto& item : *list) {
      const utils::DictionaryValue* control_dict;
      if (!item->GetAsDictionary(&control_dict) ||
          !ParseCategoryEntryAndStore(*control_dict, aplist.get())) {
        *error = kErrMsgCategory;
        return false;
      }
    }
  } else if (value->GetType() == utils::Value::TYPE_DICTIONARY) {
    // single entry
    const utils::DictionaryValue* dict;
    value->GetAsDictionary(&dict);
    if (!ParseCategoryEntryAndStore(*dict, aplist.get()))
      return false;
  } else {
    LOG(INFO) << "Category element is not defined.";
    return true;
  }

  application->SetManifestData(keys::kTizenCategoryKey, aplist);
  return true;
}

bool CategoryHandler::Validate(
    std::shared_ptr<const ApplicationData> application,
    std::string* error) const {
  const CategoryInfoList* categories_list =
      static_cast<const CategoryInfoList*>(
          application->GetManifestData(keys::kTizenCategoryKey));

  for (const auto& item : categories_list->categories) {
    if (item.empty()) {
      *error = kErrMsgCategoryName;
      return false;
    }
  }
  return true;
}

std::vector<std::string> CategoryHandler::Keys() const {
  return std::vector<std::string>(1, keys::kTizenCategoryKey);
}

}  // namespace widget_manifest_parser
}  // namespace common_installer
