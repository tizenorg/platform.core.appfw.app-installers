// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/pkgmgr/step_update_pkg_disable_info.h"
#include "common/pkgmgr_registration.h"

#if 0
#include <unistd.h>

#include <cassert>
#include <string>


#include "common/pkgmgr_query.h"
#include "common/utils/file_util.h"
#endif
namespace common_installer {
namespace pkgmgr {

StepUpdatePkgDisableInfo::StepUpdatePkgDisableInfo(
    common_installer::InstallerContext* context, ActionType action_type)
    : Step(context), action_type_(action_type) {}

Step::Status StepUpdatePkgDisableInfo::precheck() {
  if (context_->pkgid.get().empty()) {
    LOG(ERROR) << "pkgid attribute is empty";
    return Status::PACKAGE_NOT_FOUND;
  }

  return Step::Status::OK;
}

Step::Status StepUpdatePkgDisableInfo::process() {
  if (action_type_ ==  ActionType::Disable) {
    if (!DisablePkgInPkgmgr(context_->pkgid.get(), context_->uid.get(), context_->request_mode.get())) {
      LOG(ERROR) << "Failed to update disable pkg info :" << context_->pkgid.get();
      return Status::REGISTER_ERROR;
    }
  } else {
    if (!EnablePkgInPkgmgr(context_->pkgid.get(), context_->uid.get(), context_->request_mode.get())) {
      LOG(ERROR) << "Failed to update disable pkg info :" << context_->pkgid.get();
      return Status::REGISTER_ERROR;
    }
  }
  LOG(DEBUG) << "Successfully set pkg enable/disable info";
  return Status::OK;
}

Step::Status StepUpdatePkgDisableInfo::undo() {
  if (action_type_ ==  ActionType::Disable) {
    if (!EnablePkgInPkgmgr(context_->pkgid.get(), context_->uid.get(), context_->request_mode.get())) {
      LOG(ERROR) << "Failed to update enable pkg info :" << context_->pkgid.get();
      return Status::REGISTER_ERROR;
    }
  } else {
    if (!DisablePkgInPkgmgr(context_->pkgid.get(), context_->uid.get(), context_->request_mode.get())) {
      LOG(ERROR) << "Failed to update disable pkg info :" << context_->pkgid.get();
      return Status::REGISTER_ERROR;
    }
  }
  LOG(DEBUG) << "Successfully undo pkg enable/disable info";
  return Status::OK;
}

}  // namespace pkgmgr
}  // namespace common_installer
