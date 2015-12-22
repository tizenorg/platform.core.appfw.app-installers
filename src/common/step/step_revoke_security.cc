// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_revoke_security.h"

#include <boost/filesystem.hpp>

#include "common/security_registration.h"

namespace common_installer {
namespace security {

Step::Status StepRevokeSecurity::precheck() {
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

Step::Status StepRevokeSecurity::clean() {
  std::string error_message;
  if (!UnregisterSecurityContextForManifest(
      context_->pkgid.get(),  context_->uid.get(),
      context_->manifest_data.get(), &error_message)) {
    LOG(ERROR) << "Failure on unregistering security context for app "
               << context_->pkgid.get();
    if (!error_message.empty()) {
      LOG(ERROR) << "error_message: " << error_message;
      on_error(Status::SECURITY_ERROR, error_message);
    }
    return Status::SECURITY_ERROR;
  }
  LOG(DEBUG) << "Security context uninstalled";
  return Status::OK;
}

}  // namespace security
}  // namespace common_installer
