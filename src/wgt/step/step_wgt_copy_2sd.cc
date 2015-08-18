// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include <app2ext_interface.h>

#include <string>

#include "common/wgt_external_storage.h"
#include "wgt/step/step_wgt_copy_2sd.h"

namespace ci = common_installer;

namespace wgt {
namespace filesystem {

common_installer::Step::Status StepWgtCopy2SD::precheck() {
  LOG(DEBUG) << "WgtCopy2SD precheck started";
  pkgid_ = context_->pkgid.get().c_str();
  backend_data_ = static_cast<WgtBackendData*>(context_->backend_data.get());
  if (!backend_data_) {
    LOG(ERROR) << "No backend data";
    return common_installer::Step::Status::ERROR;
  }
  return StepCopy::precheck();
}

common_installer::Step::Status StepWgtCopy2SD::process() {
  LOG(DEBUG) << "WgtCopy2SD process started";
  if (IsToInstallExternal(backend_data_->settings.get().install_location())) {
    Step::Status status;
    ci::WgtExternalStorage* storage = new ci::WgtExternalStorage();
    if (storage->PreInstallation(pkgid_)) {
      LOG(DEBUG) << "Preinstallation executed succesfully, postinstallation";
      status = StepCopy::process();
      storage->PostInstallation(APP2EXT_STATUS_SUCCESS, pkgid_);
    } else {
        LOG(DEBUG) << "Preinstallation failed, normal installation";
        status = StepCopy::process();
        storage->PostInstallation(APP2EXT_STATUS_FAILED, pkgid_);
      }
    storage->~WgtExternalStorage();
    return status;
  } else {
      LOG(DEBUG) << "Normal installation";
      return StepCopy::process();
  }
}

common_installer::Step::Status StepWgtCopy2SD::undo() {
  LOG(DEBUG) << "WgtCopy2SD undo started";
    Step::Status status;
    ci::WgtExternalStorage* storage = new ci::WgtExternalStorage();
    if (storage->PreUninstallation(pkgid_)) {
      status = StepCopy::undo();
      storage->PostUninstallation(pkgid_);
    } else {
      status = StepCopy::undo();
      storage->PostUninstallation(pkgid_);
    }
    storage->~WgtExternalStorage();
    return status;
}

common_installer::Step::Status StepWgtCopy2SD::clean() {
  LOG(DEBUG) << "WgtCopy2SD clean started";
  return StepCopy::clean();
}

bool StepWgtCopy2SD::IsToInstallExternal(parse::SettingInfo::InstallLocation
                                         location) {
  return location == parse::SettingInfo::InstallLocation::EXTERNAL;
}

}  //  namespace filesystem
}  //  namespace wgt


