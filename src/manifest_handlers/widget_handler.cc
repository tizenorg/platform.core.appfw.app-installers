// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "manifest_handlers/widget_handler.h"

#include <string.h>

#include <cassert>
#include <map>
#include <utility>
#include <set>

#include "parser/manifest_constants.h"
#include "parser/values.h"
#include "manifest_handlers/application_manifest_constants.h"

namespace wgt {
namespace parse {

namespace keys = wgt::application_widget_keys;

namespace {
// Below key names are readable from Javascript widget interface.
const char kAuthor[] = "author";
const char kDecription[] = "description";
const char kName[] = "name";
const char kShortName[] = "shortName";
const char kVersion[] = "version";
const char kID[] = "id";
const char kAuthorEmail[] = "authorEmail";
const char kAuthorHref[] = "authorHref";
const char kHeight[] = "height";
const char kWidth[] = "width";
const char kPreferences[] = "preferences";

// Child keys inside 'preferences' key.
const char kPreferencesName[] = "name";
const char kPreferencesValue[] = "value";
const char kPreferencesReadonly[] = "readonly";

typedef std::map<std::string, std::string> KeyMap;
typedef std::map<std::string, std::string>::const_iterator KeyMapIterator;
typedef std::pair<std::string, std::string> KeyPair;

const KeyMap& GetWidgetKeyPairs() {
  static KeyMap map;
  if (map.empty()) {
    map.insert(KeyPair(keys::kAuthorKey, kAuthor));
    map.insert(KeyPair(keys::kDescriptionKey, kDecription));
    map.insert(KeyPair(keys::kNameKey, kName));
    map.insert(KeyPair(keys::kShortNameKey, kShortName));
    map.insert(KeyPair(keys::kVersionKey, kVersion));
    map.insert(KeyPair(keys::kIDKey, kID));
    map.insert(KeyPair(keys::kAuthorEmailKey, kAuthorEmail));
    map.insert(KeyPair(keys::kAuthorHrefKey, kAuthorHref));
    map.insert(KeyPair(keys::kHeightKey, kHeight));
    map.insert(KeyPair(keys::kWidthKey, kWidth));
  }

  return map;
}

bool ParsePreferenceItem(const parser::utils::DictionaryValue* in_value,
                         parser::utils::DictionaryValue* out_value,
                         std::set<std::string>* used) {
  assert(in_value && in_value->IsType(parser::utils::Value::TYPE_DICTIONARY));

  std::string pref_name;
  std::string pref_value;
  std::string pref_readonly;
  if (in_value->GetString(keys::kPreferencesNameKey, &pref_name)
     && used->find(pref_name) == used->end()) {
    out_value->SetString(kPreferencesName, pref_name);
    used->insert(pref_name);
  } else {
    return false;
  }

  if (in_value->GetString(keys::kPreferencesValueKey, &pref_value))
    out_value->SetString(kPreferencesValue, pref_value);

  if (in_value->GetString(keys::kPreferencesReadonlyKey, &pref_readonly))
    out_value->SetBoolean(kPreferencesReadonly, pref_readonly == "true");
  return true;
}

}  // namespace

WidgetInfo::WidgetInfo()
    : value_(new parser::utils::DictionaryValue) {}

WidgetInfo::~WidgetInfo() {}

void WidgetInfo::SetString(const std::string& key, const std::string& value) {
  value_->SetString(key, value);
}

void WidgetInfo::Set(const std::string& key, parser::utils::Value* value) {
  value_->Set(key, value);
}

void WidgetInfo::SetName(const std::string& name) {
  value_->SetString(kName, name);
}

void WidgetInfo::SetShortName(const std::string& short_name) {
  value_->SetString(kShortName, short_name);
}

void WidgetInfo::SetDescription(const std::string& description) {
  value_->SetString(kDecription, description);
}

WidgetHandler::WidgetHandler() {}

WidgetHandler::~WidgetHandler() {}

bool WidgetHandler::Parse(
    const parser::Manifest& manifest,
    std::shared_ptr<parser::ManifestData>* output,
    std::string* /*error*/) {
  std::shared_ptr<WidgetInfo> widget_info(new WidgetInfo());
  const KeyMap& map = GetWidgetKeyPairs();

  for (KeyMapIterator iter = map.begin(); iter != map.end(); ++iter) {
    std::string string;
    bool result = manifest.GetString(iter->first, &string);
    if (result && !string.empty() && iter->first == keys::kAuthorHrefKey)
      // When authorhref is an invalid URI, reset it an empty string.
      string.clear();
    widget_info->SetString(iter->second, result ? string : "");
  }

  parser::utils::Value* pref_value = nullptr;
  manifest.Get(keys::kPreferencesKey, &pref_value);

  std::set<std::string> preference_names_used;
  if (pref_value && pref_value->IsType(parser::utils::Value::TYPE_DICTIONARY)) {
    parser::utils::DictionaryValue* preferences =
        new parser::utils::DictionaryValue;
    parser::utils::DictionaryValue* dict;
    pref_value->GetAsDictionary(&dict);
    if (ParsePreferenceItem(dict, preferences, &preference_names_used))
      widget_info->Set(kPreferences, preferences);
  } else if (pref_value && pref_value->IsType(
                 parser::utils::Value::TYPE_LIST)) {
    parser::utils::ListValue* preferences = new parser::utils::ListValue;
    parser::utils::ListValue* list;
    pref_value->GetAsList(&list);

    for (parser::utils::ListValue::iterator it = list->begin();
         it != list->end(); ++it) {
      parser::utils::DictionaryValue* pref = new parser::utils::DictionaryValue;
      parser::utils::DictionaryValue* dict;
      (*it)->GetAsDictionary(&dict);
      if (ParsePreferenceItem(dict, pref, &preference_names_used))
        preferences->Append(pref);
    }
    widget_info->Set(kPreferences, preferences);
  }
  std::string ns_value;
  if (manifest.GetString(keys::kWidgetNamespaceKey, &ns_value)) {
    widget_info->SetString(keys::kWidgetNamespaceKey, ns_value);
  }

  *output = std::static_pointer_cast<parser::ManifestData>(widget_info);
  return true;
}

bool WidgetHandler::Validate(
    const parser::ManifestData& data,
    const parser::ManifestDataMap& /*handlers_output*/,
    std::string* error) const {
  const WidgetInfo& widget_info = static_cast<const WidgetInfo&>(data);
  std::string ns_value;
  if (!widget_info.GetWidgetInfo()->GetString(keys::kWidgetNamespaceKey,
                                               &ns_value)) {
    *error = "Failed to retrieve the widget's namespace.";
    return false;
  }
  if (strcasecmp(keys::kWidgetNamespacePrefix, ns_value.c_str()) != 0) {
    *error = "The widget namespace is invalid.";
    return false;
  }
  return true;
}

bool WidgetHandler::AlwaysParseForType() const {
  return true;
}

std::string WidgetHandler::Key() const {
  return application_widget_keys::kTizenWidgetKey;
}

}  // namespace parse
}  // namespace wgt
