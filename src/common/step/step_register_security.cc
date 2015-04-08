// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_register_security.h"

#include <boost/filesystem.hpp>

#include "common/security_registration.h"

namespace common_installer {
namespace security {

#define FIELD_CHECK(field)  \
  if (field) { \
    LOG(ERROR) << #field " attribute is empty"; \
    return Step::Status::INVALID_VALUE; \
  }

#define PATH_CHECK(dir)  \
  FIELD_CHECK(dir.empty()) \
  if (!boost::filesystem::exists(dir)) { \
    LOG(ERROR) << #dir " (" << dir << ") path does not exist"; \
    return Step::Status::INVALID_VALUE; \
  }

Step::Status StepRegisterSecurity::precheck() {
  PATH_CHECK(context_->application_path.get())
  FIELD_CHECK(context_->pkgid.get().empty())
  FIELD_CHECK(context_->manifest_data.get())

  return Step::Status::OK;
}

#undef PATH_CHECK
#undef FIELD_CHECK

Step::Status StepRegisterSecurity::process() {
  if (!RegisterSecurityContextForApps(
      context_->pkgid.get(), context_->application_path.get(),
      context_->manifest_data.get())) {
    return Status::ERROR;
  }
  LOG(DEBUG) << "Security context installed";
  return Status::OK;
}

Step::Status StepRegisterSecurity::undo() {
  if (!UnregisterSecurityContextForApps(
      context_->pkgid.get(), context_->manifest_data.get())) {
    return Status::ERROR;
  }
  LOG(DEBUG) << "Security context uninstalled";
  return Status::OK;
}

}  // namespace security
}  // namespace common_installer
