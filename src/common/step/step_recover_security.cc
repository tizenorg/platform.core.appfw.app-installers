// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_recover_security.h"

#include <boost/filesystem.hpp>

#include "common/security_registration.h"

namespace common_installer {
namespace security {

bool StepRecoverSecurity::Check() {
  if (context_->pkg_path.get().empty())
    return false;
  if (!boost::filesystem::exists(context_->pkg_path.get()))
    return false;
  if (context_->pkgid.get().empty())
    return false;
  if (!context_->manifest_data.get())
    return false;
  return true;
}

Step::Status StepRecoverSecurity::RecoveryNew() {
  if (!Check())
    return Status::OK;
  UnregisterSecurityContext(context_->pkgid.get());
  return Status::OK;
}

Step::Status StepRecoverSecurity::RecoveryUpdate() {
  if (!Check()) {
    LOG(ERROR) << "Invalid parameters";
    return Status::ERROR;
  }
  if (!RegisterSecurityContext(
      context_->pkgid.get(), context_->pkg_path.get(),
      context_->manifest_data.get())) {
    LOG(ERROR) << "Unsuccessful update";
    return Status::ERROR;
  }
  return Status::OK;
}
}  // namespace security
}  // namespace common_installer
