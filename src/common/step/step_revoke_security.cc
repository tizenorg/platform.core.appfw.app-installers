// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_revoke_security.h"

#include "common/security_registration.h"

namespace common_installer {
namespace revoke_security {

Step::Status StepRevokeSecurity::process() {
  if (!UnregisterSecurityContextForApps(
      context_->pkgid(), context_->manifest_data())) {
    LOG(ERROR) << "Failure on unregistering security context for app "
               << context_->pkgid();
    return Status::ERROR;
  }
  LOG(DEBUG) << "Security context uninstalled";
  return Status::OK;
}

Step::Status StepRevokeSecurity::undo() {
  if (!RegisterSecurityContextForApps(
      context_->pkgid(), context_->GetRootApplicationPath(),
      context_->manifest_data())) {
    LOG(DEBUG) << "Error while reverting security changes";
  }
  // always return positive result to let other steps do the undo() operation
  return Status::OK;
}

}  // namespace revoke_security
}  // namespace common_installer
