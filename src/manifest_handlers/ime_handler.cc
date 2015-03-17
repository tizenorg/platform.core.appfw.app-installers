// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "manifest_handlers/ime_handler.h"

#include <cassert>
#include <memory>
#include <regex>

#include "utils/logging.h"
#include "parser/values.h"
#include "manifest_handlers/application_manifest_constants.h"

namespace wgt {
namespace parse {

namespace keys = wgt::application_widget_keys;

namespace {

const char kErrMsgLanguages[] =
    "At least and only ONE tizen:languages tag should be specified";
const char kErrMsgEmptyLanguage[] =
    "Language cannot be empty";
const char kErrMsgParsingIme[] =
    "Only one ime tag should be specified";
const char kErrMsgParsingUuid[] =
    "Only one uuid tag should be specified";
const char kErrMsgValidatingUuidEmpty[] =
    "The UUID of ime element is obligatory";
const char kErrMsgUuidFormat[] =
    "Uuid should be in proper format (8-4-4-4-12)";
const char kErrMsgNoLanguages[] =
    "At least one language of ime element should be specified";

const std::regex kUuidRegex(
    "^[0-9a-zA-Z]{8}([-][0-9a-zA-Z]{4}){3}[-][0-9a-zA-Z]{12}$");

bool GetLanguage(const parser::utils::Value* item, ImeInfo* ime_info,
    std::string* error) {
  const parser::utils::DictionaryValue* language_dict;
  if (item->GetAsDictionary(&language_dict)) {
    std::string language;
    if (!language_dict->GetString(keys::kTizenImeLanguageTextKey, &language) ||
        language.empty()) {
      *error = kErrMsgEmptyLanguage;
      return false;
    }
    ime_info->AddLanguage(language);
  }
  return true;
}

bool ParseImeEntryAndStore(const parser::utils::DictionaryValue& control_dict,
    ImeInfo* ime_info, std::string* error) {

  // parsing uuid element
  const parser::utils::DictionaryValue* uuid_dict;
  std::string uuid;
  if (control_dict.GetDictionary(keys::kTizenImeUuidKey, &uuid_dict) &&
      uuid_dict->GetString(keys::kTizenImeUuidTextKey, &uuid)) {
    ime_info->set_uuid(uuid);
  } else {
    *error = kErrMsgParsingUuid;
    return false;
  }

  const parser::utils::DictionaryValue* languages_dict;
  if (!control_dict.GetDictionary(
      keys::kTizenImeLanguagesKey, &languages_dict)) {
    *error = kErrMsgLanguages;
    return false;
  }

  const parser::utils::Value* languages;
  if (!languages_dict->Get(keys::kTizenImeLanguageKey, &languages)) {
    *error = kErrMsgNoLanguages;
    return false;
  }

  if (languages->GetType() == parser::utils::Value::TYPE_LIST) {
    // many languages
    const parser::utils::ListValue* list;
    languages->GetAsList(&list);
    for (const auto& item : *list) {
      if (!GetLanguage(item, ime_info, error))
        return false;
    }
  } else if (languages->GetType() == parser::utils::Value::TYPE_DICTIONARY) {
    if (!GetLanguage(languages, ime_info, error))
      return false;
  }

  return true;
}

// UUID is string of 36 characters in form 8-4-4-4-12
bool IsValidUuid(const std::string& uuid) {
  return std::regex_match(uuid, kUuidRegex);
}

}  // namespace

ImeInfo::ImeInfo() {
}

ImeInfo::~ImeInfo() {
}

ImeHandler::ImeHandler() {
}

ImeHandler::~ImeHandler() {
}

void ImeInfo::AddLanguage(const std::string& language) {
  languages_.push_back(language);
}

bool ImeHandler::Parse(
    const parser::Manifest& manifest,
    std::shared_ptr<parser::ManifestData>* output,
    std::string* error) {
  std::shared_ptr<ImeInfo> ime_info(new ImeInfo);
  parser::utils::Value* value;
  manifest.Get(keys::kTizenImeKey, &value);

  bool result = true;

  if (value->GetType() == parser::utils::Value::TYPE_DICTIONARY) {
    // single entry
    const parser::utils::DictionaryValue* dict;
    value->GetAsDictionary(&dict);
    result = ParseImeEntryAndStore(*dict, ime_info.get(), error);
  } else if (value->GetType() == parser::utils::Value::TYPE_LIST) {
    *error = kErrMsgParsingIme;
    return nullptr;
  } else {
    LOG(INFO) << "IME element is not defined.";
    return true;
  }

  *output = std::static_pointer_cast<parser::ManifestData>(ime_info);
  return true;
}

bool ImeHandler::Validate(
    const parser::ManifestData& data,
    const parser::ManifestDataMap& /*handlers_output*/,
    std::string* error) const {

  const ImeInfo& ime_info = static_cast<const ImeInfo&>(data);

  if (!ime_info.exists())
    return true;

  if (ime_info.uuid().empty()) {
    *error = kErrMsgValidatingUuidEmpty;
    return false;
  }

  if (!IsValidUuid(ime_info.uuid())) {
    *error = kErrMsgUuidFormat;
    return false;
  }

  if (ime_info.languages().empty()) {
    *error = kErrMsgNoLanguages;
    return false;
  }

  return true;
}

std::string ImeHandler::Key() const {
  return keys::kTizenImeKey;
}

}  // namespace parse
}  // namespace wgt
