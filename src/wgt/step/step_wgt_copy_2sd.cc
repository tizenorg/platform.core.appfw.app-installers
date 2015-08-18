// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_wgt_copy_2sd.h"

namespace ci = common_installer;

namespace wgt {
namespace filesystem {

ci::Step::Status StepWgtCopy2SD::precheck() {
  LOG(DEBUG) << "WgtCopy2SD precheck started";
  pkgid_ = context_->pkgid.get();
  backend_data_ = static_cast<WgtBackendData*>(context_->backend_data.get());
  if (!backend_data_) {
    LOG(ERROR) << "No backend data";
    return ci::Step::Status::ERROR;
  }
  return StepCopy::precheck();
}

ci::Step::Status StepWgtCopy2SD::process() {
  LOG(DEBUG) << "WgtCopy2SD process started";
  installed_external = false;
  if (!IsToInstallExternal()) {
    LOG(DEBUG) << "Standard installation";
    return StepCopy::process();
  }
  // external installation
  storage_ = std::unique_ptr<ci::WgtExternalStorage>(
        new ci::WgtExternalStorage(pkgid_));
  if (!storage_->IsInitialized()) {
    LOG(DEBUG) << "Cannot initialize external storage";
    return StepCopy::process();
  }
  ci::Step::Status status;
  if (!storage_->PreInstallation()) {
    LOG(DEBUG) << "Preinstallation failed";
    status = StepCopy::process();
    storage_->PostInstallation(false);
  } else {
    status = StepCopy::process();
    storage_->PostInstallation(true);
    installed_external = true;
  }
  return status;
}

ci::Step::Status StepWgtCopy2SD::undo() {
  LOG(DEBUG) << "WgtCopy2SD undo started";
  if (!installed_external) {
    return StepCopy::undo();
  }
  // TODO(a.niznik) To be checked, which of below methods should be used.
  // storage_->PostInstallation(false);
  // return StepCopy::undo();
  Step::Status status;
  storage_->PreUninstallation();
  status = StepCopy::undo();
  storage_->PostUninstallation();
  return status;
}

ci::Step::Status StepWgtCopy2SD::clean() {
  LOG(DEBUG) << "WgtCopy2SD clean started";
  // if (installed_external) {
  //  storage_->PostInstallation(true);
  // }
  return StepCopy::clean();
}

bool StepWgtCopy2SD::IsToInstallExternal() {
  return parse::SettingInfo::InstallLocation::EXTERNAL ==
        backend_data_->settings.get().install_location();
}

}  //  namespace filesystem
}  //  namespace wgt


