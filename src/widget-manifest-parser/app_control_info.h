// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef WIDGET_MANIFEST_PARSER_APP_CONTROL_INFO_H_
#define WIDGET_MANIFEST_PARSER_APP_CONTROL_INFO_H_

#include <string>
#include <vector>

#include "widget-manifest-parser/application_data.h"

namespace common_installer {
namespace widget_manifest_parser {

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

struct AppControlInfoList : public ApplicationData::ManifestData {
  std::vector<AppControlInfo> controls;
};

}   // namespace widget_manifest_parser
}   // namespace common_installer

#endif  // WIDGET_MANIFEST_PARSER_APP_CONTROL_INFO_H_
