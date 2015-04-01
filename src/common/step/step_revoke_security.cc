// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_revoke_security.h"

#include "common/security_registration.h"

namespace common_installer {
namespace revoke_security {

Step::Status StepRevokeSecurity::process() {
  if (!UnregisterSecurityContextForApps(
      context_->pkgid.get(), context_->manifest_data.get())) {
    LOG(ERROR) << "Failure on unregistering security context for app "
               << context_->pkgid.get();
    return Status::ERROR;
  }
  LOG(DEBUG) << "Security context uninstalled";
  return Status::OK;
}

Step::Status StepRevokeSecurity::undo() {
  if (!RegisterSecurityContextForApps(
      context_->pkgid.get(), context_->application_path.get(),
      context_->manifest_data.get())) {
    LOG(ERROR) << "Failure on re-installing security context for app "
               << context_->pkgid.get();
    return Status::ERROR;
  }
  LOG(DEBUG) << "Security context installed";
  return Status::OK;
}

}  // namespace revoke_security
}  // namespace common_installer
