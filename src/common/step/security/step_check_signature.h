// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_SECURITY_STEP_CHECK_SIGNATURE_H_
#define COMMON_STEP_SECURITY_STEP_CHECK_SIGNATURE_H_

#include <manifest_parser/utils/logging.h>

#include <string>

#include "common/installer_context.h"
#include "common/step/step.h"

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
  __attribute__ ((visibility ("default"))) Status process() override;

  __attribute__ ((visibility ("default"))) Status undo() override { return Status::OK; }

  __attribute__ ((visibility ("default"))) Status clean() override { return Status::OK; }

  /**
   * \brief checks if unpacked dir is available
   *
   * \return Status::OK if available, Status::ERRO otherwise
   */
  __attribute__ ((visibility ("default"))) Status precheck() override;

  SCOPE_LOG_TAG(Signature)
};

// Exposed for tests
__attribute__ ((visibility ("default"))) Step::Status ValidateSignatures(const boost::filesystem::path& base_path,
    PrivilegeLevel* level, common_installer::CertificateInfo* cert_info,
    bool check_reference, std::string* error_message);

}  // namespace security
}  // namespace common_installer

#endif  // COMMON_STEP_SECURITY_STEP_CHECK_SIGNATURE_H_
