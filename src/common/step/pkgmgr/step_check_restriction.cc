// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/pkgmgr/step_check_restriction.h"

#include <package-manager.h>

#include "common/installer_context.h"

namespace ci = common_installer;

namespace common_installer {
namespace pkgmgr {

Step::Status StepCheckRestriction::process() {
  int type;
  switch (context_->request_type.get()) {
  case ci::RequestType::Install:
    type = PM_RESTRICTION_MODE_INSTALL;
    break;
  case ci::RequestType::Uninstall:
    type = PM_RESTRICTION_MODE_UNINSTALL;
    break;
  case ci::RequestType::Reinstall:
    type = PM_RESTRICTION_MODE_REINSTALL;
    break;
  case ci::RequestType::Move:
    type = PM_RESTRICTION_MODE_MOVE;
    break;
  default:
    type = -1;
    break;
  }

  if (type < 0)
    return Status::OK;

  pkgmgr_client* pc = pkgmgr_client_new(PC_REQUEST);
  if (!pc)
    return Status::ERROR;

  int mode;
  int ret = pkgmgr_client_usr_get_restriction_mode(
      pc, &mode, context_->uid.get());
  if (ret != PKGMGR_R_OK) {
    LOG(ERROR) << "Failed to get restriction mode bit";
    pkgmgr_client_free(pc);
    return Status::ERROR;
  }

  if (type & mode) {
    LOG(ERROR) << "Restricted operation";
    pkgmgr_client_free(pc);
    return Status::OPERATION_NOT_ALLOWED;
  }

  ret = pkgmgr_client_usr_get_pkg_restriction_mode(
      pc, context_->pkgid.get().c_str(), &mode, context_->uid.get());
  if (ret != PKGMGR_R_OK) {
    LOG(ERROR) << "Failed to get pkg(" << context_->pkgid.get()
               << ") restriction mode bit";
    pkgmgr_client_free(pc);
    return Status::ERROR;
  }

  if (type & mode) {
    LOG(ERROR) << "Restricted operation for pkgid: " << context_->pkgid.get();
    pkgmgr_client_free(pc);
    return Status::OPERATION_NOT_ALLOWED;
  }

  pkgmgr_client_free(pc);

  return Status::OK;
}

}  // namespace pkgmgr
}  // namespace common_installer
