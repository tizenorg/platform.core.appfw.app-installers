// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#ifndef COMMON_MANIFEST_INFO_BACKGROUND_CATEGORY_H_
#define COMMON_MANIFEST_INFO_BACKGROUND_CATEGORY_H_

#include <string>
#include <vector>

namespace common_installer {

/**
 * \brief Details about background-category
 */
struct BackgroundCategoryInfo {
  std::string appid;
  std::string providerid;

  std::string value;
};

/**
 * \brief Contains all background-category instances
 */
class BackgroundCategoryContainer {
 public:
  /**
   * \brief Default constructor
   */
  BackgroundCategoryContainer() {}

  /**
   * \brief Accessor method for background categories details
   *
   * \return Reference for vector of BackgroundCategoryInfo
   */
  const std::vector<BackgroundCategoryInfo>& background_categories() const {
    return background_categories_;
  }

  /**
   * \brief Adds BackgroundCategoryInfo object to the collection
   *
   * \param background_category details about background-category element
   */
  void add_background_category(
      const BackgroundCategoryInfo& background_category) {
    background_categories_.push_back(background_category);
  }

 private:
  std::vector<BackgroundCategoryInfo> background_categories_;
};

} // namespace common_installer

#endif /* COMMON_MANIFEST_INFO_BACKGROUND_CATEGORY_H_ */
