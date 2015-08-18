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
  location_ = backend_data_->settings.get().install_location();
  return StepCopy::precheck();
}

ci::Step::Status StepWgtCopy2SD::process() {
  LOG(DEBUG) << "WgtCopy2SD process started";
  if (!IsToInstallExternal()) {
    LOG(DEBUG) << "Standard installation";
    return StepCopy::process();
  }
  // external installation
  storage_ = new ci::WgtExternalStorage();
  if (!storage_->IsInitialized()) {
    LOG(DEBUG) << "Cannot initialize external storage";
    return StepCopy::process();
  }
  Step::Status status;
  if (!storage_->PreInstallation(pkgid_)) {
    LOG(DEBUG) << "Preinstallation failed";
    status = StepCopy::process();
    storage_->PostInstallation(false, pkgid_);
    LOG(DEBUG) << "Postinstallation failed";
  } else {
    status = StepCopy::process();
    storage_->PostInstallation(true, pkgid_);
    }
    return status;
}

ci::Step::Status StepWgtCopy2SD::undo() {
  LOG(DEBUG) << "WgtCopy2SD undo started";
  if (!IsToInstallExternal()) {
    return StepCopy::undo();
  }
  if (!storage_->PreUninstallation(pkgid_)) {
    LOG(DEBUG) << "Preuninstallation failed";
    delete storage_;
    return StepCopy::undo();
  }
  Step::Status status;
  status = StepCopy::undo();
  storage_->PostUninstallation(pkgid_);
  delete storage_;
  return status;
}

ci::Step::Status StepWgtCopy2SD::clean() {
  LOG(DEBUG) << "WgtCopy2SD clean started";
  if (storage_) {
  delete storage_;
  }
  return StepCopy::clean();
}

bool StepWgtCopy2SD::IsToInstallExternal() {
  return location_ == parse::SettingInfo::InstallLocation::EXTERNAL;
}

}  //  namespace filesystem
}  //  namespace wgt


