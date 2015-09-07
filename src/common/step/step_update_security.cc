// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_update_security.h"

#include "common/security_registration.h"

namespace common_installer {
namespace security {

Step::Status StepUpdateSecurity::process() {
  if (!RegisterSecurityContextForManifest(
      context_->pkgid.get(), context_->pkg_path.get(), context_->uid.get(),
      context_->manifest_data.get())) {
    return Status::ERROR;
  }
  LOG(DEBUG) << "Security context updated";
  return Status::OK;
}

Step::Status StepUpdateSecurity::undo() {
  if (!RegisterSecurityContextForManifest(
      context_->pkgid.get(), context_->pkg_path.get(), context_->uid.get(),
      context_->old_manifest_data.get())) {
    return Status::ERROR;
  }
  LOG(DEBUG) << "Security context reverted";
  return Status::OK;
}

}  // namespace security
}  // namespace common_installer
