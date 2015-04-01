// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_register_security.h"

#include "common/security_registration.h"

namespace common_installer {
namespace security {

Step::Status StepRegisterSecurity::process() {
  if (!RegisterSecurityContextForApps(
      context_->pkgid.get(), context_->application_path.get(),
      context_->manifest_data.get())) {
    return Status::ERROR;
  }
  LOG(DEBUG) << "Security context installed";
  return Status::OK;
}

Step::Status StepRegisterSecurity::undo() {
  if (!UnregisterSecurityContextForApps(
      context_->pkgid.get(), context_->manifest_data.get())) {
    return Status::ERROR;
  }
  LOG(DEBUG) << "Security context uninstalled";
  return Status::OK;
}

}  // namespace security
}  // namespace common_installer
