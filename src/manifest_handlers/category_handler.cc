// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "manifest_handlers/category_handler.h"

#include "utils/logging.h"
#include "parser/values.h"
#include "manifest_handlers/application_manifest_constants.h"

namespace wgt {
namespace parse {

namespace keys = wgt::application_widget_keys;

namespace {

const char kErrMsgCategory[] =
    "Parsing category element failed";
const char kErrMsgCategoryName[] =
    "The name element inside category element is obligatory";

bool ParseCategoryEntryAndStore(
    const parser::utils::DictionaryValue& control_dict,
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

bool CategoryHandler::Parse(
    const parser::Manifest& manifest,
    std::shared_ptr<parser::ManifestData>* output,
    std::string* error) {
  std::shared_ptr<CategoryInfoList> aplist(new CategoryInfoList());
  parser::utils::Value* value;
  manifest.Get(keys::kTizenCategoryKey, &value);

  if (value->GetType() == parser::utils::Value::TYPE_LIST) {
    // multiple entries
    const parser::utils::ListValue* list;
    value->GetAsList(&list);
    for (const auto& item : *list) {
      const parser::utils::DictionaryValue* control_dict;
      if (!item->GetAsDictionary(&control_dict) ||
          !ParseCategoryEntryAndStore(*control_dict, aplist.get())) {
        *error = kErrMsgCategory;
        return nullptr;
      }
    }
  } else if (value->GetType() == parser::utils::Value::TYPE_DICTIONARY) {
    // single entry
    const parser::utils::DictionaryValue* dict;
    value->GetAsDictionary(&dict);
    if (!ParseCategoryEntryAndStore(*dict, aplist.get()))
      return nullptr;
  } else {
    LOG(INFO) << "Category element is not defined.";
    return true;
  }
  *output = std::static_pointer_cast<parser::ManifestData>(aplist);
  return true;
}

bool CategoryHandler::Validate(
    const parser::ManifestData& data,
    const parser::ManifestDataMap& handlers_output,
    std::string* error) const {
  const CategoryInfoList& categories_list =
      static_cast<const CategoryInfoList&>(data);

  for (const auto& item : categories_list.categories) {
    if (item.empty()) {
      *error = kErrMsgCategoryName;
      return false;
    }
  }
  return true;
}

std::string CategoryHandler::Key() const {
  return keys::kTizenCategoryKey;
}

}  // namespace parse
}  // namespace wgt
