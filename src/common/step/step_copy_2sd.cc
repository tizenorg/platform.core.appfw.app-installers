// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_copy_2sd.h"

namespace common_installer {
namespace filesystem {

Step::Status StepCopy2SD::precheck() {
  LOG(DEBUG) << "Copy2SD precheck started";
  pkgid_ = context_->pkgid.get();
  manifest_ = context_->manifest_data.get();
  if (!manifest_) {
    LOG(ERROR) << "No manifest data";
    return Step::Status::ERROR;
  }
  return StepCopy::precheck();
}

Step::Status StepCopy2SD::process() {
  LOG(DEBUG) << "Copy2SD process started";
  installed_external = false;
  if (!IsToInstallExternal()) {
    LOG(DEBUG) << "Standard installation";
    return StepCopy::process();
  }
  // external installation
  storage_ = std::unique_ptr<ExternalStorage>(
        new ExternalStorage(pkgid_));
  if (!storage_->IsInitialized()) {
    LOG(DEBUG) << "Cannot initialize external storage";
    return StepCopy::process();
  }
  Step::Status status;
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

Step::Status StepCopy2SD::undo() {
  LOG(DEBUG) << "Copy2SD undo started";
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

Step::Status StepCopy2SD::clean() {
  LOG(DEBUG) << "Copy2SD clean started";
  // if (installed_external) {
  //  storage_->PostInstallation(true);
  // }
  return StepCopy::clean();
}

bool StepCopy2SD::IsToInstallExternal() {
  return
      (strcmp(manifest_->installlocation, "prefer-external") == 0);
}

}  //  namespace filesystem
}  //  namespace common_installer


