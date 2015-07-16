// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_CHECK_SIGNATURE_H_
#define COMMON_STEP_STEP_CHECK_SIGNATURE_H_

#include "common/context_installer.h"
#include "common/step/step.h"
#include "common/utils/logging.h"

namespace common_installer {
namespace security {

class StepCheckSignature : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status undo() override { return Status::OK; }
  Status clean() override { return Status::OK; }
  Status precheck() override;

  SCOPE_LOG_TAG(Signature)
};

// Exposed for tests
Step::Status ValidateSignatures(const boost::filesystem::path& base_path,
    PrivilegeLevel* level, common_installer::CertificateInfo* cert_info);

}  // namespace security
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_CHECK_SIGNATURE_H_
