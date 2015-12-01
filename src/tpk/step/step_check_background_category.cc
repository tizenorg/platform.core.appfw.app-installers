// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "tpk/step/step_check_background_category.h"

#include "common/installer_context.h"

namespace tpk {
namespace security {

StepCheckBackgroundCategory::StepCheckBackgroundCategory(
    common_installer::InstallerContext *context) :
        BaseStepCheckBackgroundCategory(context) {};

bool StepCheckBackgroundCategory::GetBackgroundSupport() {
  return true;
}

}  // namespace security
}  // namespace tpk
