// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_rollback_installation_security.h"

#include <boost/filesystem.hpp>
#include <string>

#include "common/security_registration.h"

namespace common_installer {
namespace security {

Step::Status StepRollbackInstallationSecurity::precheck() {
  if (context_->pkgid.get().empty()) {
    LOG(ERROR) << "pkgid attribute is empty";
    return Step::Status::PACKAGE_NOT_FOUND;
  }
  if (!context_->manifest_data.get()) {
    LOG(ERROR) << "manifest_data attribute is empty";
    return Step::Status::MANIFEST_NOT_FOUND;
  }

  return Step::Status::OK;
}

Step::Status StepRollbackInstallationSecurity::undo() {
  std::string error_message;
  if (!UnregisterSecurityContextForManifest(
      context_->pkgid.get(),  context_->uid.get(),
      context_->manifest_data.get(), &error_message)) {
    if (!error_message.empty()) {
      LOG(ERROR) << "error_message: " << error_message;
    }
    return Status::SECURITY_ERROR;
  }
  LOG(DEBUG) << "Security context uninstalled";
  return Status::OK;
}

}  // namespace security
}  // namespace common_installer
