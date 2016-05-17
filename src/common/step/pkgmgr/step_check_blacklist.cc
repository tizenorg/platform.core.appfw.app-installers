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
  if (context_->installation_mode.get() == InstallationMode::OFFLINE ||
      context_->is_preload_request.get())
    return Status::OK;
  int result;
  pkgmgr_client* pc = pkgmgr_client_new(PC_REQUEST);

  if (!pc)
    return Status::ERROR;

  int ret = context_->request_mode.get() != RequestMode::GLOBAL ?
      pkgmgr_client_usr_get_pkg_restriction_mode(pc, context_->pkgid.get().c_str(),
          context_->uid.get(), &result) :
      pkgmgr_client_get_pkg_restriction_mode(pc, context_->pkgid.get().c_str(),
          &result);
  pkgmgr_client_free(pc);
  if (ret != PKGMGR_R_OK)
    return Status::ERROR;

  common_installer::RequestType type = context_->request_type.get();
  switch (type) {
    case common_installer::RequestType::Install:
    case common_installer::RequestType::Update:
      if (result & PM_RESTRICTION_MODE_INSTALL) {
        LOG(ERROR) << "This package is blacklisted";
        return Status::OPERATION_NOT_ALLOWED;
      }
      break;
    case common_installer::RequestType::Reinstall:
      if (result & PM_RESTRICTION_MODE_REINSTALL) {
        LOG(ERROR) << "This package is blacklisted";
        return Status::OPERATION_NOT_ALLOWED;
      }
      break;
    case common_installer::RequestType::Uninstall:
      if (result & PM_RESTRICTION_MODE_UNINSTALL) {
        LOG(ERROR) << "This package is blacklisted";
        return Status::OPERATION_NOT_ALLOWED;
      }
      break;
    default:
      return Status::OK;
  }

  // TODO(jungh.yeon) : add user check routine

  return Status::OK;
}

Step::Status StepCheckBlacklist::precheck() {
  if (context_->pkgid.get().empty())
    return Status::INVALID_VALUE;
  return Status::OK;
}

}  // namespace pkgmgr
}  // namespace common_installer
