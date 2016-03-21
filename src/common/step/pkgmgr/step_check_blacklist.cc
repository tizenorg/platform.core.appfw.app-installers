// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/pkgmgr/step_check_blacklist.h"

#include <package-manager.h>

#include "common/app_installer.h"
#include "common/step/step.h"

namespace common_installer {
namespace pkgmgr {

Step::Status StepCheckBlacklist::process() {
  if (context_->installation_mode.get() == InstallationMode::OFFLINE)
    return Status::OK;
  bool result;
  pkgmgr_client* pc = pkgmgr_client_new(PC_REQUEST);
  if (!pc)
    return Status::ERROR;
  int ret = context_->request_mode.get() != RequestMode::GLOBAL ?
      pkgmgr_client_usr_check_blacklist(pc, context_->pkgid.get().c_str(),
          &result, context_->uid.get()) :
      pkgmgr_client_check_blacklist(pc, context_->pkgid.get().c_str(),
          &result);
  pkgmgr_client_free(pc);
  if (ret != PKGMGR_R_OK)
    return Status::ERROR;

  if (result) {
    LOG(ERROR) << "This package is blacklisted";
    return Status::OPERATION_NOT_ALLOWED;
  }

  return Status::OK;
}

Step::Status StepCheckBlacklist::precheck() {
  if (context_->pkgid.get().empty())
    return Status::INVALID_VALUE;
  return Status::OK;
}

}  // namespace pkgmgr
}  // namespace common_installer
