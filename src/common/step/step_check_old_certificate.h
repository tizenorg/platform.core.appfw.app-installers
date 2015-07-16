// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_CHECK_OLD_CERTIFICATE_H_
#define COMMON_STEP_STEP_CHECK_OLD_CERTIFICATE_H_

#include "common/context_installer.h"
#include "common/step/step.h"
#include "common/utils/logging.h"

namespace common_installer {
namespace security {

class StepCheckOldCertificate : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status undo() override { return Status::OK; }
  Status clean() override { return Status::OK; }
  Status precheck() override { return Status::OK; }

  SCOPE_LOG_TAG(CheckOldCertificate)
};

}  // namespace security
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_CHECK_OLD_CERTIFICATE_H_
