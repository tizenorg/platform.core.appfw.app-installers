// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_revoke_security.h"

#include <iostream>

#include "common/security_registration.h"

// TODO(t.iwanek): logging mechanism...
#define DBG(msg) std::cout << "[RevokeSecurity] " << msg << std::endl;
#define ERR(msg) std::cout << "[ERROR:RevokeSecurity] " << msg << std::endl;

namespace common_installer {
namespace revoke_security {

Step::Status StepRevokeSecurity::process() {
  if (!UnregisterSecurityContextForApps(
      context_->pkgid(), context_->uid(), context_->manifest_data())) {
    return Status::ERROR;
  }
  DBG("Security context uninstalled");
  return Status::OK;
}

Step::Status StepRevokeSecurity::undo() {
  if (!RegisterSecurityContextForApps(
      context_->pkgid(), context_->uid(), context_->GetRootApplicationPath(),
      context_->manifest_data())) {
    return Status::ERROR;
  }
  DBG("Security context installed");
  return Status::OK;
}

}  // namespace revoke_security
}  // namespace common_installer
