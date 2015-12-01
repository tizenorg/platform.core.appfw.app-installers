// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef TPK_STEP_STEP_CHECK_BACKGROUND_CATEGORY_H_
#define TPK_STEP_STEP_CHECK_BACKGROUND_CATEGORY_H_

#include <manifest_parser/utils/version_number.h>

#include "common/step/step_check_background_category.h"

namespace tpk {
namespace security {

/**
 * \brief This step check background category value and modify it depending on
 *        required version, cert level, background support, and value itself
 */
class StepCheckBackgroundCategory :
    public common_installer::security::BaseStepCheckBackgroundCategory {
 public:
  explicit StepCheckBackgroundCategory(
      common_installer::InstallerContext *context);

 protected:
  bool GetBackgroundSupport() override;
};

}  // namespace security
}  // namespace tpk

#endif  // TPK_STEP_STEP_CHECK_BACKGROUND_CATEGORY_H_
