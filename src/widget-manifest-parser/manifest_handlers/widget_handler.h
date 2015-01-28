// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.
// Part of this file is redistributed from crosswalk project under BSD-style
// license. Check LICENSE-xwalk file.

#ifndef WIDGET_MANIFEST_PARSER_MANIFEST_HANDLERS_WIDGET_HANDLER_H_
#define WIDGET_MANIFEST_PARSER_MANIFEST_HANDLERS_WIDGET_HANDLER_H_

#include <map>
#include <string>
#include <vector>

#include "utils/macros.h"
#include "utils/values.h"
#include "widget-manifest-parser/application_data.h"
#include "widget-manifest-parser/manifest_handler.h"

namespace common_installer {
namespace widget_manifest_parser {

class WidgetInfo : public ApplicationData::ManifestData {
 public:
  WidgetInfo();
  virtual ~WidgetInfo();
  void SetString(const std::string& key, const std::string& value);
  void Set(const std::string& key, utils::Value* value);

  // Name, shrot name and description are i18n items, they will be set
  // if their value were changed after loacle was changed.
  void SetName(const std::string& name);
  void SetShortName(const std::string& short_name);
  void SetDescription(const std::string& description);

  utils::DictionaryValue* GetWidgetInfo() {
    return value_.get();
  }

 private:
  std::unique_ptr<utils::DictionaryValue> value_;
};

class WidgetHandler : public ManifestHandler {
 public:
  WidgetHandler();
  virtual ~WidgetHandler();

  bool Parse(std::shared_ptr<ApplicationData> application,
             std::u16string* error) override;
  bool AlwaysParseForType(Manifest::Type type) const override;
  std::vector<std::string> Keys() const override;

  bool Validate(std::shared_ptr<const ApplicationData> application,
                std::string* error) const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(WidgetHandler);
};

}  // namespace widget_manifest_parser
}  // namespace common_installer

#endif  // WIDGET_MANIFEST_PARSER_MANIFEST_HANDLERS_WIDGET_HANDLER_H_
