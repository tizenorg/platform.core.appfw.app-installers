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
  // FIXME remove setting app_id at all
  // this is broken for multiple apps in one package
  if (!context_->manifest_data()->uiapplication ||
      !context_->manifest_data()->uiapplication->appid) {
    return Status::ERROR;
  }
  if (!RegisterSecurityContext(context_->manifest_data()->uiapplication->appid,
      context_->pkgid(), context_->uid(), context_->GetRootApplicationPath(),
      context_->manifest_data())) {
    return Status::ERROR;
  }
  DBG("Security context installed");
  return Status::OK;
}

Step::Status StepSecurity::undo() {
  // FIXME remove setting app_id at all
  // this is broken for multiple apps in one package
  if (!context_->manifest_data()->uiapplication ||
      !context_->manifest_data()->uiapplication->appid) {
    return Status::ERROR;
  }
  if (!UnregisterSecurityContext(
      context_->manifest_data()->uiapplication->appid,
      context_->pkgid(), context_->uid())) {
    return Status::ERROR;
  }
  DBG("Security context uninstalled");
  return Status::OK;
}

}  // namespace security
}  // namespace common_installer
