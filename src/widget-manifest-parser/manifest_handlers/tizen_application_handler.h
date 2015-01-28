// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.
// Part of this file is redistributed from crosswalk project under BSD-style
// license. Check LICENSE-xwalk file.

#ifndef WIDGET_MANIFEST_PARSER_MANIFEST_HANDLERS_TIZEN_APPLICATION_HANDLER_H_
#define WIDGET_MANIFEST_PARSER_MANIFEST_HANDLERS_TIZEN_APPLICATION_HANDLER_H_

#include <map>
#include <string>
#include <vector>

#include "utils/macros.h"
#include "utils/values.h"
#include "widget-manifest-parser/application_manifest_constants.h"
#include "widget-manifest-parser/manifest_handler.h"

namespace common_installer {
namespace widget_manifest_parser {

class TizenApplicationInfo : public ApplicationData::ManifestData {
 public:
  TizenApplicationInfo();
  virtual ~TizenApplicationInfo();

  void set_id(const std::string& id) {
    id_ = id;
  }
  void set_package(const std::string& package) {
    package_ = package;
  }
  void set_required_version(
      const std::string& required_version) {
    required_version_ = required_version;
  }
  const std::string& id() const {
    return id_;
  }
  const std::string& package() const {
    return package_;
  }
  const std::string& required_version() const {
    return required_version_;
  }

 private:
  std::string id_;
  std::string package_;
  std::string required_version_;
};

class TizenApplicationHandler : public ManifestHandler {
 public:
  TizenApplicationHandler();
  virtual ~TizenApplicationHandler();

  bool Parse(std::shared_ptr<ApplicationData> application,
             std::u16string* error) override;
  bool Validate(std::shared_ptr<const ApplicationData> application,
                std::string* error) const override;
  bool AlwaysParseForType(Manifest::Type type) const override;
  std::vector<std::string> Keys() const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(TizenApplicationHandler);
};

}  // namespace widget_manifest_parser
}  // namespace common_installer

#endif  // WIDGET_MANIFEST_PARSER_MANIFEST_HANDLERS_TIZEN_APPLICATION_HANDLER_H_
