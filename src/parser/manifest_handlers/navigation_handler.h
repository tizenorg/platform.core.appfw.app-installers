// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef PARSER_MANIFEST_HANDLERS_NAVIGATION_HANDLER_H_
#define PARSER_MANIFEST_HANDLERS_NAVIGATION_HANDLER_H_

#include <memory>
#include <string>
#include <vector>

#include "parser/application_data.h"
#include "parser/manifest_handler.h"

namespace common_installer {
namespace parser {

class NavigationInfo : public ApplicationData::ManifestData {
 public:
  explicit NavigationInfo(const std::string& allowed_domains);
  virtual ~NavigationInfo();

  const std::vector<std::string>& GetAllowedDomains() const {
    return allowed_domains_;
  }

 private:
  std::vector<std::string> allowed_domains_;
};

class NavigationHandler : public ManifestHandler {
 public:
  NavigationHandler();
  virtual ~NavigationHandler();

  bool Parse(std::shared_ptr<ApplicationData> application_data,
             std::string* error) override;
  std::vector<std::string> Keys() const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(NavigationHandler);
};

}  // namespace parser
}  // namespace common_installer

#endif  // PARSER_MANIFEST_HANDLERS_NAVIGATION_HANDLER_H_
