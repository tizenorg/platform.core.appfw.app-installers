// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_security.h"

#include "common/security_registration.h"

namespace common_installer {
namespace security {

Step::Status StepSecurity::process() {
  if (!RegisterSecurityContextForApps(
      context_->pkgid(), context_->GetApplicationPath(),
      context_->manifest_data())) {
    return Status::ERROR;
  }
  LOG(DEBUG) << "Security context installed";
  return Status::OK;
}

}  // namespace security
}  // namespace common_installer
