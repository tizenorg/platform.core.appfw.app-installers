// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/pkgmgr/step_check_restriction.h"

#include <package-manager.h>

#include "common/app_installer.h"
#include "common/step/step.h"

namespace {
  bool IsRestricted(common_installer::RequestType type, int mode) {

    switch(type) {
    case common_installer::RequestType::Install:
    case common_installer::RequestType::Update:
    case common_installer::RequestType::Delta:
    case common_installer::RequestType::MountInstall:
    case common_installer::RequestType::MountUpdate:
      if (mode & PM_RESTRICTION_MODE_INSTALL)
        return true;
      break;

    case common_installer::RequestType::Reinstall:
      if (mode & PM_RESTRICTION_MODE_REINSTALL)
        return true;
      break;

    default:
        return false;
        break;
    }

    return false;
  }
} // namespace

namespace common_installer {
namespace pkgmgr {

Step::Status StepCheckRestriction::process() {
  int mode = 0;

  pkgmgr_client* pc = pkgmgr_client_new(PC_REQUEST);
  if (!pc)
    return Status::ERROR;

  int ret = context_->request_mode.get() != RequestMode::GLOBAL ?
      pkgmgr_client_usr_get_pkg_restriction_mode(pc, context_->pkgid.get().c_str(), context_->uid.get(), &mode) :
      pkgmgr_client_get_pkg_restriction_mode(pc, context_->pkgid.get().c_str(), &mode);
  if (ret != PKGMGR_R_OK) {
    pkgmgr_client_free(pc);
    return Status::ERROR;
  }

  if (IsRestricted(context_->request_type.get(), mode))
    return Status::OPERATION_NOT_ALLOWED;

  ret = context_->request_mode.get() != RequestMode::GLOBAL ?
      pkgmgr_client_usr_get_restriction_mode(pc, &mode, context_->uid.get()) :
      pkgmgr_client_get_restriction_mode(pc, &mode);

  pkgmgr_client_free(pc);
  if (ret != PKGMGR_R_OK) {
    return Status::ERROR;
  }

  if (IsRestricted(context_->request_type.get(), mode))
    return Status::OPERATION_NOT_ALLOWED;
}

Step::Status StepCheckRestriction::precheck() {
  if (context_->pkgid.get().empty())
    return Status::INVALID_VALUE;
  return Status::OK;
}

}  // namespace pkgmgr
}  // namespace common_installer
