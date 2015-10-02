// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/security/step_register_security.h"

#include <boost/filesystem.hpp>
#include <string>

#include "common/security_registration.h"

namespace common_installer {
namespace security {

Step::Status StepRegisterSecurity::precheck() {
  if (context_->package_storage->path().empty()) {
    LOG(ERROR) << "pkg_path attribute is empty";
    return Step::Status::INVALID_VALUE;
  }
  if (!boost::filesystem::exists(context_->package_storage->path())) {
    LOG(ERROR) << "pkg_path ("
               << context_->package_storage->path()
               << ") path does not exist";
    return Step::Status::INVALID_VALUE;
  }

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

Step::Status StepRegisterSecurity::process() {
  std::string error_message;
  if (!RegisterSecurityContextForManifest(
      context_->pkgid.get(), context_->package_storage->path(),
      context_->uid.get(), &context_->certificate_info.get(),
      context_->manifest_data.get(), &error_message)) {
    if (!error_message.empty()) {
      LOG(ERROR) << "error_message: " << error_message;
      on_error(Status::SECURITY_ERROR, error_message);
    }
    return Status::SECURITY_ERROR;
  }
  LOG(DEBUG) << "Security context installed";
  return Status::OK;
}

}  // namespace security
}  // namespace common_installer
