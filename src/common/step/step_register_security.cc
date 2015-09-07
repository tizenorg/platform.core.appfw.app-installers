// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_register_security.h"

#include <boost/filesystem.hpp>

#include "common/security_registration.h"

namespace common_installer {
namespace security {

Step::Status StepRegisterSecurity::precheck() {
  if (context_->pkg_path.get().empty()) {
    LOG(ERROR) << "pkg_path attribute is empty";
    return Step::Status::INVALID_VALUE;
  }
  if (!boost::filesystem::exists(context_->pkg_path.get())) {
    LOG(ERROR) << "pkg_path ("
               << context_->pkg_path.get()
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
  if (!RegisterSecurityContextForManifest(
      context_->pkgid.get(), context_->pkg_path.get(), context_->uid.get(),
      context_->manifest_data.get())) {
    return Status::ERROR;
  }
  LOG(DEBUG) << "Security context installed";
  return Status::OK;
}

}  // namespace security
}  // namespace common_installer
