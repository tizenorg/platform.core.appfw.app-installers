// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_security.h"

#include <iostream>

#include "common/security_registration.h"

// TODO(t.iwanek): logging mechanism...
#define DBG(msg) std::cout << "[Security] " << msg << std::endl;
#define ERR(msg) std::cout << "[ERROR:Security] " << msg << std::endl;

namespace common_installer {
namespace security {

Step::Status StepSecurity::process() {
  if (!RegisterSecurityContextForApps(
      context_->pkgid(), context_->GetRootApplicationPath(),
      context_->manifest_data())) {
    return Status::ERROR;
  }
  DBG("Security context installed");
  return Status::OK;
}

Step::Status StepSecurity::undo() {
  if (!UnregisterSecurityContextForApps(
      context_->pkgid(), context_->manifest_data())) {
    return Status::ERROR;
  }
  DBG("Security context uninstalled");
  return Status::OK;
}

}  // namespace security
}  // namespace common_installer
