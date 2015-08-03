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
    return Step::Status::INVALID_VALUE;
  }
  if (!context_->manifest_data.get()) {
    LOG(ERROR) << "manifest_data attribute is empty";
    return Step::Status::INVALID_VALUE;
  }

  return Step::Status::OK;
}

Step::Status StepRevokeSecurity::process() {
  if (!UnregisterSecurityContextForApps(
      context_->pkgid.get(), context_->manifest_data.get())) {
    LOG(ERROR) << "Failure on unregistering security context for app "
               << context_->pkgid.get();
    return Status::ERROR;
  }
  LOG(DEBUG) << "Security context uninstalled";
  return Status::OK;
}

Step::Status StepRevokeSecurity::undo() {
  if (!RegisterSecurityContextForApps(
      context_->pkgid.get(), context_->pkg_path.get(),
      context_->manifest_data.get())) {
    LOG(ERROR) << "Failure on re-installing security context for app "
               << context_->pkgid.get();
    return Status::ERROR;
  }
  LOG(DEBUG) << "Security context installed";
  return Status::OK;
}

}  // namespace security
}  // namespace common_installer