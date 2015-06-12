// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_configure.h"

#include <tzplatform_config.h>
#include <string>

#include "common/pkgmgr_interface.h"
#include "common/utils/file_util.h"

namespace common_installer {
namespace configure {

const char *kStrEmpty = "";

Step::Status StepConfigure::process() {
  PkgMgrPtr pkgmgr = PkgMgrInterface::Instance();

  if (!SetupRootAppDirectory())
    return Status::ERROR;

  switch (pkgmgr->GetRequestType()) {
    case PkgMgrInterface::Type::Install:
      context_->file_path.set(pkgmgr->GetRequestInfo());
      context_->pkgid.set(kStrEmpty);
      break;
    case PkgMgrInterface::Type::Update:
      context_->file_path.set(pkgmgr->GetRequestInfo());
      context_->pkgid.set(kStrEmpty);
      break;
    case PkgMgrInterface::Type::Uninstall:
      context_->pkgid.set(pkgmgr->GetRequestInfo());
      context_->file_path.set(kStrEmpty);
      break;
    case PkgMgrInterface::Type::Reinstall:
      context_->unpacked_dir_path.set(pkgmgr->GetRequestInfo());
      context_->pkgid.set(kStrEmpty);
      context_->file_path.set(kStrEmpty);
      break;
    default:
      // TODO(p.sikorski): should return unsupported, and display error
      LOG(ERROR) <<
          "Only installation, update and uninstallation is now supported";
      return Status::ERROR;
      break;
  }

  return Status::OK;
}

bool StepConfigure::SetupRootAppDirectory() {
  if (context_->root_application_path.get().empty()) {
    std::string root_app_path =
        context_->uid.get() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER)
          ? tzplatform_getenv(TZ_USER_APP)
          : tzplatform_getenv(TZ_SYS_RW_APP);
    if (root_app_path.empty())
      return false;

    context_->root_application_path.set(root_app_path);
  }
  if (!boost::filesystem::exists(context_->root_application_path.get())) {
    boost::system:: error_code error;
    boost::filesystem::create_directories(
        context_->root_application_path.get());
    if (error) {
      LOG(ERROR) << "Cannot create directory: "
                 << context_->root_application_path.get();
      return false;
    }
  }
  return true;
}

}  // namespace configure
}  // namespace common_installer
