// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef PARSER_MANIFEST_HANDLERS_CATEGORY_HANDLER_H_
#define PARSER_MANIFEST_HANDLERS_CATEGORY_HANDLER_H_

#include <memory>
#include <string>
#include <vector>

#include "utils/macros.h"
#include "parser/application_data.h"
#include "parser/manifest_handler.h"

namespace common_installer {
namespace parser {

struct CategoryInfoList : public ApplicationData::ManifestData {
  std::vector<std::string> categories;
};

class CategoryHandler : public ManifestHandler {
 public:
  CategoryHandler();
  virtual ~CategoryHandler();
  bool Parse(std::shared_ptr<ApplicationData> application,
      std::string* error) override;
  bool Validate(std::shared_ptr<const ApplicationData> application,
      std::string* error) const override;
  std::vector<std::string> Keys() const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(CategoryHandler);
};

}  // namespace parser
}  // namespace common_installer

#endif  // PARSER_MANIFEST_HANDLERS_CATEGORY_HANDLER_H_
