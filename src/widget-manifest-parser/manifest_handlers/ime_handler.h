// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef WIDGET_MANIFEST_PARSER_MANIFEST_HANDLERS_IME_HANDLER_H_
#define WIDGET_MANIFEST_PARSER_MANIFEST_HANDLERS_IME_HANDLER_H_

#include <memory>
#include <string>
#include <vector>

#include "utils/macros.h"
#include "widget-manifest-parser/application_data.h"
#include "widget-manifest-parser/manifest_handler.h"

namespace common_installer {
namespace widget_manifest_parser {

class ImeInfo : public ApplicationData::ManifestData {
 public:
  ImeInfo();
  virtual ~ImeInfo();

  const std::string& uuid() const {
    return uuid_;
  }
  void set_uuid(const std::string& uuid) { uuid_ = uuid; }
  const std::vector<std::string>& languages() const {
    return languages_;
  }
  void AddLanguage(const std::string& language);

 private:
  std::string uuid_;
  std::vector<std::string> languages_;
};

class ImeHandler : public ManifestHandler {
 public:
  ImeHandler();
  virtual ~ImeHandler();
  bool Parse(std::shared_ptr<ApplicationData> application,
      std::string* error) override;
  bool Validate(std::shared_ptr<const ApplicationData> application,
      std::string* error) const override;
  std::vector<std::string> Keys() const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(ImeHandler);
};

}  // namespace widget_manifest_parser
}  // namespace common_installer

#endif  // WIDGET_MANIFEST_PARSER_MANIFEST_HANDLERS_IME_HANDLER_H_
