// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_STEP_ADD_DEFAULT_PRIVILEGES_H_
#define WGT_STEP_STEP_ADD_DEFAULT_PRIVILEGES_H_

#include <manifest_parser/utils/logging.h>

#include "common/app_installer.h"
#include "common/installer_context.h"
#include "common/step/step.h"

namespace wgt {
namespace security {

/**
 * \brief Step that add default privileges during installation
 */
class StepAddDefaultPrivileges : public common_installer::Step {
 public:
  using Step::Step;

  /**
   * \brief Add default privileges
   *
   * \return Status::OK
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
   * \brief Check requirements for this step
   *
   * \return Status::ERROR when rmanifest data are missing,
   *         Status::OK otherwise
   */
  Status precheck() override;

  SCOPE_LOG_TAG(AddDefaultPrivileges)
};

}  // namespace security
}  // namespace wgt

#endif  // WGT_STEP_STEP_ADD_DEFAULT_PRIVILEGES_H_
