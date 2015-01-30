// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.
// Part of this file is redistributed from crosswalk project under BSD-style
// license. Check LICENSE-xwalk file.

#include "widget-manifest-parser/manifest.h"

#include <algorithm>
#include <list>

#include "utils/system_locale.h"
#include "widget-manifest-parser/application_manifest_constants.h"

namespace errors = common_installer::application_manifest_errors;
namespace keys   = common_installer::application_manifest_keys;
namespace widget_keys = common_installer::application_widget_keys;

namespace common_installer {
namespace widget_manifest_parser {
namespace {
const char kLocaleUnlocalized[] = "@unlocalized";
const char kLocaleAuto[] = "en-gb";
const char kLocaleFirstOne[] = "*";

const char kWidgetNamePath[] = "widget.name";
const char kWidgetDecriptionPath[] = "widget.description";
const char kWidgetLicensePath[] = "widget.license";

const char kPathConnectSymbol = '.';

typedef std::list<std::string> List;

std::string GetLocalizedKey(const std::string& key,
                            const std::string& local) {
  std::string lower_local;
  std::transform(local.begin(), local.end(), lower_local.begin(), ::tolower);
  if (lower_local.empty())
    lower_local = kLocaleUnlocalized;
  return key + kPathConnectSymbol + lower_local;
}

std::unique_ptr<List> ExpandUserAgentLocalesList(
    const std::unique_ptr<List>& list) {
  std::unique_ptr<List> expansion_list(new List);
  for (List::const_iterator it = list->begin(); it != list->end(); ++it) {
    std::string copy_locale(*it);
    size_t position;
    do {
      expansion_list->push_back(copy_locale);
      position = copy_locale.find_last_of("-");
      copy_locale = copy_locale.substr(0, position);
    } while (position != std::string::npos);
  }
  return std::move(expansion_list);
}

}  // namespace

Manifest::Manifest(std::unique_ptr<utils::DictionaryValue> value, Type type)
    : data_(std::move(value)),
      i18n_data_(new utils::DictionaryValue),
      type_(type) {

  if (data_->HasKey(widget_keys::kWidgetKey) &&
      data_->Get(widget_keys::kWidgetKey, nullptr))
    ParseWGTI18n();

  // FIXME: Sounds like a setter calling a getter for the same value.
  SetSystemLocale(utils::GetSystemLocale());
}

Manifest::~Manifest() {
}

bool Manifest::ValidateManifest(
    std::string* error) const {
  // TODO(xiang): support features validation
  return true;
}

bool Manifest::HasKey(const std::string& key) const {
  return CanAccessKey(key) && data_->HasKey(key);
}

bool Manifest::HasPath(const std::string& path) const {
  utils::Value* ignored = utils::Value::CreateNullValue();
  return CanAccessPath(path) && data_->Get(path, &ignored);
}

bool Manifest::Get(
    const std::string& path, const utils::Value** out_value) const {
  return CanAccessPath(path) && data_->Get(path, out_value);
}

bool Manifest::Get(
    const std::string& path, utils::Value** out_value) const {
  return this->Get(
      path,
      const_cast<const utils::Value**>(out_value));
}

bool Manifest::GetBoolean(
    const std::string& path, bool* out_value) const {
  return CanAccessPath(path) && data_->GetBoolean(path, out_value);
}

bool Manifest::GetInteger(
    const std::string& path, int* out_value) const {
  return CanAccessPath(path) && data_->GetInteger(path, out_value);
}

bool Manifest::GetString(
    const std::string& path, std::string* out_value) const {
  if (!CanAccessPath(path))
    return false;

  if (i18n_data_->Get(path, nullptr)) {
    List::const_iterator it = user_agent_locales_->begin();
    for (; it != user_agent_locales_->end(); ++it) {
      if (i18n_data_->GetString(GetLocalizedKey(path, *it), out_value))
        return true;
    }
    return false;
  }

  return data_->GetString(path, out_value);
}

bool Manifest::GetDictionary(
    const std::string& path, const utils::DictionaryValue** out_value) const {
  return CanAccessPath(path) && data_->GetDictionary(path, out_value);
}

bool Manifest::GetList(
    const std::string& path, const utils::ListValue** out_value) const {
  return CanAccessPath(path) && data_->GetList(path, out_value);
}

Manifest* Manifest::DeepCopy() const {
  Manifest* manifest = new Manifest(
      std::unique_ptr<utils::DictionaryValue>(data_->DeepCopy()),
      type());
  return manifest;
}

bool Manifest::Equals(const Manifest* other) const {
  return other && data_->Equals(other->value());
}

bool Manifest::CanAccessPath(const std::string& path) const {
  return true;
}

bool Manifest::CanAccessKey(const std::string& key) const {
  return true;
}

void Manifest::SetSystemLocale(const std::string& locale) {
  std::unique_ptr<List> list_for_expand(new List);
  list_for_expand->push_back(locale);
  list_for_expand->push_back(default_locale_);
  list_for_expand->push_back(kLocaleUnlocalized);
  list_for_expand->push_back(kLocaleAuto);
  list_for_expand->push_back(kLocaleFirstOne);
  user_agent_locales_ = ExpandUserAgentLocalesList(list_for_expand);
}

void Manifest::ParseWGTI18n() {
  data_->GetString(application_widget_keys::kDefaultLocaleKey,
                   &default_locale_);
  std::transform(default_locale_.begin(), default_locale_.end(),
                 default_locale_.begin(), ::tolower);

  ParseWGTI18nEachPath(kWidgetNamePath);
  ParseWGTI18nEachPath(kWidgetDecriptionPath);
  ParseWGTI18nEachPath(kWidgetLicensePath);
}

// We might get one element of a list of element from path,
// and we parse each element for fast access.
// For example config.xml is:
// <widget>
//   <name>unlocalized name</name>
//   <name xml:lang="zh-CN">zh-CN name</name>
//   <name xml:lang="en-US" short="en-US short">en-US name</name>
// </widget>
// The path for value in i18n_data_ are :
// "widget.name.#text.@unlocalized" => "unlocalized name".
// "widget.name.#text.zh-cn" => "zh-CN name".
// "widget.name.#text.en-us" => "en-US name".
// "widget.name.@short.en-us" => "en-US short".
// "widget.name.#text.*" => "unlocalized name". (the first one)
// "widget.name.@short.*" => "". (the first one do not have a short name)
void Manifest::ParseWGTI18nEachPath(const std::string& path) {
  utils::Value* value = nullptr;
  if (!data_->Get(path, &value))
    return;

  if (value->IsType(utils::Value::TYPE_DICTIONARY)) {
    ParseWGTI18nEachElement(value, path);
    ParseWGTI18nEachElement(value, path, kLocaleFirstOne);
  } else if (value->IsType(utils::Value::TYPE_LIST)) {
    utils::ListValue* list;
    value->GetAsList(&list);

    bool get_first_one = false;
    for (utils::ListValue::iterator it = list->begin();
        it != list->end(); ++it) {
      ParseWGTI18nEachElement(*it, path);
      if (!get_first_one)
        get_first_one = ParseWGTI18nEachElement(*it, path, kLocaleFirstOne);
    }
  }
}

bool Manifest::ParseWGTI18nEachElement(utils::Value* value,
                                       const std::string& path,
                                       const std::string& locale) {
  utils::DictionaryValue* dict;
  if (!value->GetAsDictionary(&dict))
    return false;

  std::string xml_lang(locale);
  if (locale.empty())
    dict->GetString(application_widget_keys::kXmlLangKey, &xml_lang);

  utils::DictionaryValue::Iterator iter(*dict);
  while (!iter.IsAtEnd()) {
    std::string locale_key(
        GetLocalizedKey(path + kPathConnectSymbol + iter.key(), xml_lang));
    if (!i18n_data_->Get(locale_key, nullptr))
      i18n_data_->Set(locale_key, iter.value().DeepCopy());

    iter.Advance();
  }

  return true;
}

}  // namespace widget_manifest_parser
}  // namespace common_installer
