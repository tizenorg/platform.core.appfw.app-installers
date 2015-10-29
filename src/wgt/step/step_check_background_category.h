// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_STEP_CHECK_BACKGROUND_CATEGORY_H_
#define WGT_STEP_STEP_CHECK_BACKGROUND_CATEGORY_H_

#include "common/step/step.h"
#include "common/utils/logging.h"

namespace wgt {
namespace security {

/**
 * \brief This step check background category value and modify it depending on
 *        required version, cert level, background support, and value itself
 */
class StepCheckBackgroundCategory : public common_installer::Step {
 public:
  using Step::Step;

  /**
   * \brief Check background category values
   *
   * \return Status::ERROR when "system" detected,
   *         Status::OK otherwise
   */
  Status process() override;

  /**
   * \brief Empty method
   *
   * \return Status::OK
   */
  Status clean() override { return Status::OK; }

  /**
   * \brief Empty method
   *
   * \return Status::OK
   */
  Status undo() override { return Status::OK; }

  /**
   * \brief Empty method
   *
   * \return Status::OK
   */
  Status precheck() override { return Status::OK; }

  SCOPE_LOG_TAG(CheckBackgroundCategory)
};

}  // namespace security
}  // namespace wgt

#endif  // WGT_STEP_STEP_CHECK_BACKGROUND_CATEGORY_H_
