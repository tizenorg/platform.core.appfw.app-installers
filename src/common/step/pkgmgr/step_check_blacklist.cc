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
  int pkg_result;
  int usr_result;
  pkgmgr_client* pc = pkgmgr_client_new(PC_REQUEST);

  if (!pc)
    return Status::ERROR;

  int ret = context_->request_mode.get() != RequestMode::GLOBAL ?
      pkgmgr_client_usr_get_pkg_restriction_mode(pc, context_->pkgid.get().c_str(),
          context_->uid.get(), &pkg_result) :
      pkgmgr_client_get_pkg_restriction_mode(pc, context_->pkgid.get().c_str(),
          &pkg_result);

  if (ret != PKGMGR_R_OK) {
    pkgmgr_client_free(pc);
    return Status::ERROR;
  }

  ret = context_->request_mode.get() != RequestMode::GLOBAL ?
      pkgmgr_client_usr_get_restriction_mode(pc, &usr_result, context_->uid.get()) :
      pkgmgr_client_get_restriction_mode(pc, &usr_result);

  pkgmgr_client_free(pc);
  if (ret != PKGMGR_R_OK)
    return Status::ERROR;

  switch (context_->request_type.get()) {
    case common_installer::RequestType::Install:
    case common_installer::RequestType::Update:
      if ((pkg_result & PM_RESTRICTION_MODE_INSTALL) ||
          (usr_result & PM_RESTRICTION_MODE_INSTALL)) {
        LOG(ERROR) << "Restricted operation : INSTALL or UPDATE";
        return Status::OPERATION_NOT_ALLOWED;
      }
      break;
    case common_installer::RequestType::Reinstall:
      if ((pkg_result & PM_RESTRICTION_MODE_REINSTALL) ||
          (usr_result & PM_RESTRICTION_MODE_REINSTALL)) {
        LOG(ERROR) << "Restricted operation : REINSTALL";
        return Status::OPERATION_NOT_ALLOWED;
      }
      break;
    case common_installer::RequestType::Uninstall:
      if ((pkg_result & PM_RESTRICTION_MODE_UNINSTALL) ||
          (usr_result & PM_RESTRICTION_MODE_UNINSTALL)) {
        LOG(ERROR) << "Restricted operation : UNINSTALL";
        return Status::OPERATION_NOT_ALLOWED;
      }
      break;
    default:
      return Status::OK;
  }

  if (ret != PKGMGR_R_OK)
    return Status::ERROR;

  return Status::OK;
}

Step::Status StepCheckBlacklist::precheck() {
  if (context_->pkgid.get().empty())
    return Status::INVALID_VALUE;
  return Status::OK;
}

}  // namespace pkgmgr
}  // namespace common_installer
