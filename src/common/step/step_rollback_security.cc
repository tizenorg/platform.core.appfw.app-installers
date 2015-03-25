// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "common/step/step_rollback_security.h"

#include "common/security_registration.h"

namespace common_installer {
namespace rollback_security {

Step::Status StepRollbackSecurity::undo() {
  if (!UnregisterSecurityContextForApps(
      context_->pkgid(), context_->manifest_data())) {
    return Status::ERROR;
  }
  LOG(DEBUG) << "Security context uninstalled";
  return Status::OK;
}

}  // namespace rollback_security
}  // namespace common_installer
