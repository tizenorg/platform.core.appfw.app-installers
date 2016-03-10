// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/security/step_recover_security.h"

#include <boost/filesystem.hpp>
#include <string>

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
  std::string error_message;
  if (!UnregisterSecurityContextForManifest(
      context_->pkgid.get(), context_->uid.get(),
      context_->manifest_data.get(), &error_message)) {
    LOG(ERROR) << "Unsuccessful install";
    if (!error_message.empty()) {
      LOG(ERROR) << "error_message: " << error_message;
      on_error(Status::RECOVERY_ERROR, error_message);
    }
    return Status::RECOVERY_ERROR;
  }
  return Status::OK;
}

Step::Status StepRecoverSecurity::RecoveryUpdate() {
  if (!Check()) {
    LOG(ERROR) << "Invalid parameters";
    return Status::INVALID_VALUE;
  }
  std::string error_message;
  if (!RegisterSecurityContextForManifest(
      context_->pkgid.get(), context_->pkg_path.get(), context_->uid.get(),
      &context_->certificate_info.get(), context_->manifest_data.get(),
      &error_message)) {
    LOG(ERROR) << "Unsuccessful update";
    if (!error_message.empty()) {
      LOG(ERROR) << "error_message: " << error_message;
      on_error(Status::RECOVERY_ERROR, error_message);
    }
    return Status::RECOVERY_ERROR;
  }
  return Status::OK;
}
}  // namespace security
}  // namespace common_installer
