// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef MANIFEST_HANDLERS_CATEGORY_HANDLER_H_
#define MANIFEST_HANDLERS_CATEGORY_HANDLER_H_

#include <memory>
#include <string>
#include <vector>

#include "utils/macros.h"
#include "parser/manifest_handler.h"

namespace wgt {
namespace parse {

struct CategoryInfoList : public parser::ManifestData {
  std::vector<std::string> categories;
};

class CategoryHandler : public parser::ManifestHandler {
 public:
  CategoryHandler();
  virtual ~CategoryHandler();
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
  DISALLOW_COPY_AND_ASSIGN(CategoryHandler);
};

}  // namespace parse
}  // namespace wgt

#endif  // MANIFEST_HANDLERS_CATEGORY_HANDLER_H_
