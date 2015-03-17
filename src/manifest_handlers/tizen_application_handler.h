// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef MANIFEST_HANDLERS_TIZEN_APPLICATION_HANDLER_H_
#define MANIFEST_HANDLERS_TIZEN_APPLICATION_HANDLER_H_

#include <map>
#include <string>
#include <vector>

#include "utils/macros.h"
#include "parser/values.h"
#include "parser/manifest_handler.h"
#include "manifest_handlers/application_manifest_constants.h"

namespace wgt {
namespace parse {

class TizenApplicationInfo : public parser::ManifestData {
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

class TizenApplicationHandler : public parser::ManifestHandler {
 public:
  TizenApplicationHandler();
  virtual ~TizenApplicationHandler();

  bool Parse(
      const parser::Manifest& manifest,
      std::shared_ptr<parser::ManifestData>* output,
      std::string* error) override;
  bool Validate(
      const parser::ManifestData& data,
      const parser::ManifestDataMap& handlers_output,
      std::string* error) const override;
  bool AlwaysParseForType() const override;
  std::string Key() const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(TizenApplicationHandler);
};

}  // namespace parse
}  // namespace wgt

#endif  // MANIFEST_HANDLERS_TIZEN_APPLICATION_HANDLER_H_
