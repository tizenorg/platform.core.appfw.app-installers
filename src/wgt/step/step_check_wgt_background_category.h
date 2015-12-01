// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_STEP_CHECK_WGT_BACKGROUND_CATEGORY_H_
#define WGT_STEP_STEP_CHECK_WGT_BACKGROUND_CATEGORY_H_

#include <manifest_parser/utils/version_number.h>

#include "common/step/step_check_background_category.h"

namespace wgt {
namespace security {

namespace ci_sec = common_installer::security;

/**
 * \brief This step check background category value and modify it depending on
 *        required version, cert level, background support, and value itself
 */
class StepCheckWgtBackgroundCategory :
    public ci_sec::StepCheckBackgroundCategory {
 public:
  explicit StepCheckWgtBackgroundCategory(
      common_installer::InstallerContext *context);

 protected:
  bool GetBackgroundSupport() override;
};

}  // namespace security
}  // namespace wgt

#endif  // WGT_STEP_STEP_CHECK_WGT_BACKGROUND_CATEGORY_H_
