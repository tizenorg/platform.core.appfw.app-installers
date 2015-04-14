// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_configure.h"

#include "common/pkgmgr_interface.h"

namespace common_installer {
namespace configure {

const char *kStrEmpty = "";

Step::Status StepConfigure::process() {
  PkgMgrPtr pkgmgr = PkgMgrInterface::Instance();

  switch (pkgmgr->GetRequestType()) {
    case PkgMgrInterface::Type::Install:
      context_->file_path.set(pkgmgr->GetRequestInfo());
      context_->pkgid.set(kStrEmpty);
      break;
    case PkgMgrInterface::Type::Uninstall:
      context_->pkgid.set(pkgmgr->GetRequestInfo());
      context_->file_path.set(kStrEmpty);
      break;
    default:
      // currently, only installation and uninstallation handled
      // TODO(p.sikorski): should return unsupported, and display error
      LOG(ERROR) << "Only installation and uninstallation is now supported";
      return Status::ERROR;
      break;
  }

  return Status::OK;
}

}  // namespace configure
}  // namespace common_installer
