// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_SECURITY_STEP_CHECK_SIGNATURE_H_
#define COMMON_STEP_SECURITY_STEP_CHECK_SIGNATURE_H_

#include <string>

#include "common/installer_context.h"
#include "common/step/step.h"
#include "common/utils/logger.h"

namespace common_installer {
namespace security {

/**
 * \brief Step responsible for checking signature of given package.
 *        Used by WGT and TPK backend
 */
class StepCheckSignature : public Step {
 public:
  using Step::Step;

  /**
   * \brief main logic of checking signature
   *
   * \return Status::OK if signature correct, Status:ERROR otherwise
   */
  Status process() override;

  Status undo() override { return Status::OK; }

  Status clean() override { return Status::OK; }

  /**
   * \brief checks if unpacked dir is available
   *
   * \return Status::OK if available, Status::ERRO otherwise
   */
  Status precheck() override;

 private:
  Status CheckSignatures(bool check_reference, PrivilegeLevel* level);
  Status CheckSignatureMismatch();
  Status CheckPrivilegeLevel(PrivilegeLevel level);

  SCOPE_LOG_TAG(Signature)
};

}  // namespace security
}  // namespace common_installer

#endif  // COMMON_STEP_SECURITY_STEP_CHECK_SIGNATURE_H_
