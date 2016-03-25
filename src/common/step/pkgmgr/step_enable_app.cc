/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "common/step/pkgmgr/step_enable_app.h"

#include <unistd.h>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <string>

//#include "common/pkgmgr_registration.h"
//#include "common/utils/file_util.h"
#include "pkgmgr-info.h"

namespace common_installer {
namespace pkgmgr {

Step::Status StepEnableApplication::precheck() {

  //TODO(jungh.yeon) : should I check existance of pkg to be enabled here?
  //TODO(jungh.yeon) : pkgmgrinfo_pkginfo_get_usr_pkginfo doesn't retrieve disabled pkg.
  pkgmgrinfo_pkginfo_h handle;
  int ret = pkgmgrinfo_pkginfo_get_usr_pkginfo(context_->pkgid.get().c_str(),
      context_->uid.get(), &handle);
  if (ret != PMINFO_R_OK) {
    LOG(ERROR) << "This package is not installed";
    return Status::INVALID_VALUE;
  }

  pkgmgrinfo_pkginfo_destroy_pkginfo(handle);
  return Step::Status::OK;
}

Step::Status StepEnableApplication::process() {
  int ret = pkgmgrinfo_pkginfo_update_enabled_pkg_info_in_usr_db(
                context_->pkgid.get().c_str(), context_->uid.get());
  if (ret != PMINFO_R_OK) {
    LOG(ERROR) << "Failed to set disable pkginfo in db";
    return Status::REGISTER_ERROR;
  }

  return Status::OK;
}

Step::Status StepEnableApplication::undo() {
  int ret = pkgmgrinfo_pkginfo_update_disabled_pkg_info_in_usr_db(
                context_->pkgid.get().c_str(), context_->uid.get());
  if (ret != PMINFO_R_OK) {
    LOG(ERROR) << "Failed to set disable pkginfo in db";
    return Status::REGISTER_ERROR;
  }

  return Status::OK;
}
}  // namespace pkgmgr
}  // namespace common_installer
