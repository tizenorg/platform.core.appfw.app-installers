// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/pkgmgr/step_check_removable.h"

#include <pkgmgr-info.h>

#include "common/app_installer.h"
#include "common/step/step.h"

namespace common_installer {
namespace pkgmgr {

Step::Status StepCheckRemovable::process() {
  pkgmgrinfo_pkginfo_h handle;

  int ret = pkgmgrinfo_pkginfo_get_usr_pkginfo(context_->pkgid.get().c_str(),
      context_->uid.get(), &handle);
  if (ret != PMINFO_R_OK) {
    LOG(ERROR) << "This package is not installed";
    return Status::INVALID_VALUE;
  }

  bool removable;
  ret = pkgmgrinfo_pkginfo_is_removable(handle, &removable);
  if (ret != PMINFO_R_OK) {
    pkgmgrinfo_pkginfo_destroy_pkginfo(handle);
    return Status::INVALID_VALUE;
  }

  if (!removable) {
    LOG(ERROR) << "This package is not removable";
    pkgmgrinfo_pkginfo_destroy_pkginfo(handle);
    return Status::OPERATION_NOT_ALLOWED;
  }

  pkgmgrinfo_pkginfo_destroy_pkginfo(handle);

  return Status::OK;
}

Step::Status StepCheckRemovable::precheck() {
  if (context_->pkgid.get().empty())
    return Status::INVALID_VALUE;
  return Status::OK;
}

}  // namespace pkgmgr
}  // namespace common_installer
