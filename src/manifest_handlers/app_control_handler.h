// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef MANIFEST_HANDLERS_APP_CONTROL_HANDLER_H_
#define MANIFEST_HANDLERS_APP_CONTROL_HANDLER_H_

#include <memory>
#include <string>
#include <vector>

#include "parser/manifest.h"
#include "parser/manifest_handler.h"
#include "utils/macros.h"

namespace wgt {
namespace parse {

class AppControlHandler : public parser::ManifestHandler {
 public:
  AppControlHandler();
  ~AppControlHandler() override;
  bool Parse(
      const parser::Manifest& manifest,
      std::shared_ptr<parser::ManifestData>* output,
      std::string* error) override;
  bool Validate(
      const parser::ManifestData& data,
      const parser::ManifestDataMap& handlers_output,
      std::string* error) const override;
  std::string Key() const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(AppControlHandler);
};

class AppControlInfo {
 public:
  AppControlInfo(const std::string& src, const std::string& operation,
      const std::string& uri, const std::string& mime);
  const std::string& src() const {
    return src_;
  }
  const std::string& operation() const {
    return operation_;
  }
  const std::string& uri() const {
    return uri_;
  }
  const std::string& mime() const {
    return mime_;
  }

 private:
  std::string src_;
  std::string operation_;
  std::string uri_;
  std::string mime_;
};

struct AppControlInfoList : public parser::ManifestData {
  std::vector<AppControlInfo> controls;
};

}   // namespace parse
}   // namespace wgt

#endif  // MANIFEST_HANDLERS_APP_CONTROL_HANDLER_H_
