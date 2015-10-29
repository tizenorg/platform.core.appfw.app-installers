// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef SRC_COMMON_MANIFEST_INFO_BACKGROUND_CATEGORY_H_
#define SRC_COMMON_MANIFEST_INFO_BACKGROUND_CATEGORY_H_

#include <string>

namespace common_installer {

/**
 * Details about background-category
 */
struct BackgroundCategoryInfo {
  std::string appid;
  std::string providerid;

  // Data members
  std::string value;
};

/**
 * Contains all background-category instances
 */
class BackgroundCategoryContainer {
 public:
  BackgroundCategoryContainer() {}

  const std::vector<BackgroundCategoryInfo>& background_categories() const {
    return background_categories_;
  }

  void add_background_category(
      const BackgroundCategoryInfo& background_category) {
    background_categories_.push_back(background_category);
  }

 private:
  std::vector<BackgroundCategoryInfo> background_categories_;
};

} // namespace common_installer

#endif /* SRC_COMMON_MANIFEST_INFO_BACKGROUND_CATEGORY_H_ */
