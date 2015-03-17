// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef MANIFEST_HANDLERS_WIDGET_HANDLER_H_
#define MANIFEST_HANDLERS_WIDGET_HANDLER_H_

#include <map>
#include <string>
#include <vector>

#include "utils/macros.h"
#include "parser/values.h"
#include "parser/manifest_handler.h"

namespace wgt {
namespace parse {

class WidgetInfo : public parser::ManifestData {
 public:
  WidgetInfo();
  virtual ~WidgetInfo();
  void SetString(const std::string& key, const std::string& value);
  void Set(const std::string& key, parser::utils::Value* value);

  // Name, short name and description are i18n items, they will be set
  // if their value were changed after loacle was changed.
  void SetName(const std::string& name);
  void SetShortName(const std::string& short_name);
  void SetDescription(const std::string& description);

  parser::utils::DictionaryValue* GetWidgetInfo() const {
    return value_.get();
  }

 private:
  std::unique_ptr<parser::utils::DictionaryValue> value_;
};

class WidgetHandler : public parser::ManifestHandler {
 public:
  WidgetHandler();
  virtual ~WidgetHandler();

  bool Parse(
      const parser::Manifest& manifest,
      std::shared_ptr<parser::ManifestData>* output,
      std::string* error) override;
  bool AlwaysParseForType() const override;
  std::string Key() const override;

  bool Validate(
      const parser::ManifestData& data,
      const parser::ManifestDataMap& handlers_output,
      std::string* error) const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(WidgetHandler);
};

}  // namespace parse
}  // namespace wgt

#endif  // MANIFEST_HANDLERS_WIDGET_HANDLER_H_
