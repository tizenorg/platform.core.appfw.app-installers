// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.
// Part of this file is redistributed from crosswalk project under BSD-style
// license. Check LICENSE-xwalk file.

#ifndef WIDGET_MANIFEST_PARSER_MANIFEST_H_
#define WIDGET_MANIFEST_PARSER_MANIFEST_H_

#include <list>
#include <map>
#include <memory>
#include <string>
#include <set>
#include <vector>

#include "utils/macros.h"
#include "utils/values.h"

namespace common_installer {
namespace widget_manifest_parser {

// Wraps the DictionaryValue form of application's manifest. Enforces access to
// properties of the manifest using ManifestFeatureProvider.
class Manifest {
 public:
  enum Type {
    TYPE_MANIFEST,  // Corresponds to w3c.github.io/manifest
    TYPE_WIDGET     // Corresponds to http://www.w3.org/TR/widgets
  };

  explicit Manifest(
      std::unique_ptr<utils::DictionaryValue> value, Type type = TYPE_MANIFEST);
  ~Manifest();

  // Returns false and |error| will be non-empty if the manifest is malformed.
  // |warnings| will be populated if there are keys in the manifest that cannot
  // be specified by the application type.
  bool ValidateManifest(std::string* error) const;

  // Returns the manifest type.
  Type type() const { return type_; }

  // These access the wrapped manifest value, returning false when the property
  // does not exist or if the manifest type can't access it.
  bool HasKey(const std::string& key) const;
  bool HasPath(const std::string& path) const;
  bool Get(const std::string& path, const utils::Value** out_value) const;
  bool Get(const std::string& path, utils::Value** out_value) const;
  bool GetBoolean(const std::string& path, bool* out_value) const;
  bool GetInteger(const std::string& path, int* out_value) const;

  // If the path is supported by i18n, we can get a locale string from
  // this two GetString function. The following is locale priority:
  // Application locale (locale get from system).                 | high
  // Default locale (defaultlocale attribute of widget element)
  // Unlocalized (the element without xml:lang attribute)
  // Auto ("en-us"(tizen is "en-gb") will be considered as a default)
  // First (the worst case we get the first element)              | low
  bool GetString(const std::string& path, std::string* out_value) const;

  bool GetDictionary(const std::string& path,
                     const utils::DictionaryValue** out_value) const;
  bool GetList(const std::string& path,
               const utils::ListValue** out_value) const;

  // Returns a new Manifest equal to this one, passing ownership to
  // the caller.
  Manifest* DeepCopy() const;

  // Returns true if this equals the |other| manifest.
  bool Equals(const Manifest* other) const;

  // Gets the underlying DictionaryValue representing the manifest.
  // Note: only use this when you KNOW you don't need the validation.
  const utils::DictionaryValue* value() const { return data_.get(); }

  const std::string& default_locale() const {
    return default_locale_;
  }

  // Update user agent locale when system locale is changed.
  void SetSystemLocale(const std::string& locale);

 private:
  void ParseWGTI18n();
  void ParseWGTI18nEachPath(const std::string& path);
  bool ParseWGTI18nEachElement(utils::Value* value,
                               const std::string& path,
                               const std::string& locale = "");

  // Returns true if the application can specify the given |path|.
  bool CanAccessPath(const std::string& path) const;
  bool CanAccessKey(const std::string& key) const;

  // Unique package id for tizen platform
  std::string package_id_;

  // The underlying dictionary representation of the manifest.
  std::unique_ptr<utils::DictionaryValue> data_;
  std::unique_ptr<utils::DictionaryValue> i18n_data_;

  std::string default_locale_;
  std::unique_ptr<std::list<std::string> > user_agent_locales_;

  Type type_;

  DISALLOW_COPY_AND_ASSIGN(Manifest);
};

}  // namespace widget_manifest_parser
}  // namespace common_installer

#endif  // WIDGET_MANIFEST_PARSER_MANIFEST_H_
