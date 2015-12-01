// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_CHECK_OLD_CERTIFICATE_H_
#define COMMON_STEP_STEP_CHECK_OLD_CERTIFICATE_H_

#include <manifest_parser/utils/logging.h>

#include "common/installer_context.h"
#include "common/step/step.h"

namespace common_installer {
namespace security {

/**
 * \brief step responsible for checking and comparing certificates of
 *        current and new version of application. Used by WGT and TPK
 */
class StepCheckOldCertificate : public Step {
 public:
  using Step::Step;

  /**
   * \brief main checking/comparing logic.
   *
   * \return Status::OK if certificates are OK. ERROR otherwise
   */
  Status process() override;

  /**
   * \brief empty method
   *
   * \return Status::OK
   */
  Status undo() override { return Status::OK; }

  /**
   * \brief empty method
   *
   * \return Status::OK
   */
  Status clean() override { return Status::OK; }

  /**
   * \brief empty method
   *
   * \return Status::OK
   */
  Status precheck() override { return Status::OK; }

  SCOPE_LOG_TAG(CheckOldCertificate)
};

}  // namespace security
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_CHECK_OLD_CERTIFICATE_H_
