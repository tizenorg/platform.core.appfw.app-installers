// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "parser/manifest_handlers/metadata_handler.h"

#include <cassert>
#include <map>
#include <utility>

#include "utils/values.h"
#include "parser/application_manifest_constants.h"

namespace common_installer {
namespace parser {

namespace keys = application_widget_keys;

typedef std::pair<std::string, std::string> MetaDataPair;
typedef std::map<std::string, std::string> MetaDataMap;
typedef std::map<std::string, std::string>::const_iterator MetaDataIter;

namespace {

MetaDataPair ParseMetaDataItem(const utils::DictionaryValue* dict,
                               std::string* error) {
  assert(dict && dict->IsType(utils::Value::TYPE_DICTIONARY));
  MetaDataPair result;
  if (!dict->GetString(keys::kTizenMetaDataNameKey, &result.first) ||
      !dict->GetString(keys::kTizenMetaDataValueKey, &result.second)) {
    *error = "Invalid key/value of tizen metaData.";
  }

  return result;
}

}  // namespace

MetaDataInfo::MetaDataInfo() {}

MetaDataInfo::~MetaDataInfo() {}

bool MetaDataInfo::HasKey(const std::string& key) const {
  return metadata_.find(key) != metadata_.end();
}

std::string MetaDataInfo::GetValue(const std::string& key) const {
  MetaDataIter it = metadata_.find(key);
  if (it != metadata_.end())
    return it->second;
  return std::string("");
}

void MetaDataInfo::SetValue(const std::string& key,
                                 const std::string& value) {
  metadata_.insert(MetaDataPair(key, value));
}

MetaDataHandler::MetaDataHandler() {}

MetaDataHandler::~MetaDataHandler() {}

bool MetaDataHandler::Parse(std::shared_ptr<ApplicationData> application,
                                 std::string* error) {
  std::shared_ptr<MetaDataInfo> metadata_info(new MetaDataInfo);
  const Manifest* manifest = application->GetManifest();
  assert(manifest);

  utils::Value* metadata_value = NULL;
  if (!manifest->Get(keys::kTizenMetaDataKey, &metadata_value)) {
    *error = "Failed to get value of tizen metaData";
  }

  MetaDataPair metadata_item;
  if (metadata_value && metadata_value->IsType(utils::Value::TYPE_DICTIONARY)) {
    utils::DictionaryValue* dict;
    metadata_value->GetAsDictionary(&dict);
    metadata_item = ParseMetaDataItem(dict, error);
    metadata_info->SetValue(metadata_item.first, metadata_item.second);
  } else if (metadata_value &&
        metadata_value->IsType(utils::Value::TYPE_LIST)) {
    utils::ListValue* list;
    metadata_value->GetAsList(&list);

    for (utils::ListValue::iterator it = list->begin();
         it != list->end(); ++it) {
      utils::DictionaryValue* dict;
      (*it)->GetAsDictionary(&dict);
      metadata_item = ParseMetaDataItem(dict, error);
      metadata_info->SetValue(metadata_item.first, metadata_item.second);
    }
  }

  application->SetManifestData(keys::kTizenMetaDataKey,
                               metadata_info);
  return true;
}

std::vector<std::string> MetaDataHandler::Keys() const {
  return std::vector<std::string>(1, keys::kTizenMetaDataKey);
}

}  // namespace parser
}  // namespace common_installer
