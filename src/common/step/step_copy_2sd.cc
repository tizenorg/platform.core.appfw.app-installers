// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_copy_2sd.h"

namespace common_installer {
namespace filesystem {

Step::Status StepCopy2SD::precheck() {
  LOG(DEBUG) << "Copy2SD precheck started";
  if (!context_->manifest_data.get()) {
    LOG(ERROR) << "No manifest data";
    return Step::Status::ERROR;
  }
  return StepCopy::precheck();
}

Step::Status StepCopy2SD::process() {
  LOG(DEBUG) << "Copy2SD process started";
  if (!IsToInstallExternal()) {
    LOG(DEBUG) << "Standard installation";
    installed_external = false;
    return StepCopy::process();
  }
  // external installation
  storage_ = std::unique_ptr<ExternalStorage>(
        new ExternalStorage(context_->pkgid.get()));
  if (!storage_->IsInitialized()) {
    LOG(WARNING) << "Cannot initialize external storage";
    return StepCopy::process();
  }
  Step::Status status;
  if (!storage_->PreInstallation()) {
    LOG(DEBUG) << "Preinstallation failed";
    context_->preinstall.set(false);
    status = StepCopy::process();
  } else {
    status = StepCopy::process();
    context_->preinstall.set(true);
  }
  return status;
}

Step::Status StepCopy2SD::undo() {
  LOG(DEBUG) << "Copy2SD undo started";
  if (!installed_external) {
    return StepCopy::undo();
  }
  if (!context_->preinstall.get()) {
    storage_->PostInstallation(false);
    return StepCopy::undo();
  }
  storage_->PostInstallation(true);
  return StepCopy::undo();
}

Step::Status StepCopy2SD::clean() {
  LOG(DEBUG) << "Copy2SD clean started";
  return StepCopy::clean();
}

bool StepCopy2SD::IsToInstallExternal() {
  return strcmp(context_->manifest_data.get()->installlocation,
                "prefer-external") == 0;
}

}  //  namespace filesystem
}  //  namespace common_installer


