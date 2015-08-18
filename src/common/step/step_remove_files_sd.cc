// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_remove_files_sd.h"

namespace common_installer {
namespace filesystem {

Step::Status StepRemoveFilesSD::precheck() {
  LOG(DEBUG) << "RemoveFilesFromSD precheck started";
  return StepRemoveFiles::precheck();
}

Step::Status StepRemoveFilesSD::process() {
  LOG(DEBUG) << "RemoveFilesFromSD process started";
  pkgid_ = context_->pkgid.get();
  installed_external_ = false;
  Step::Status status;
  Result res = CheckLocation();
  switch (res) {
  case Result::EXTERNAL: {
    LOG(DEBUG) << "Uninstallation from external storage";
    storage_ = std::unique_ptr<ExternalStorage>(
          new ExternalStorage(pkgid_));
    if (!storage_->IsInitialized()) {
      LOG(ERROR) << "Error in uninstallation";
      return Step::Status::ERROR;
    }
    if (!storage_->PreUninstallation()) {
      LOG(DEBUG) << "Preuninstallation failed";
      return Step::Status::ERROR;
    } else {
      LOG(DEBUG) << "Preuninstallation succeeded";
      status = StepRemoveFiles::process();
      storage_->PostUninstallation();
      installed_external_ = true;
      }
  }
    return status;
  case Result::INTERNAL:
    LOG(DEBUG) << "Normal uninstallation";
    return StepRemoveFiles::process();
  default:
    LOG(DEBUG) << "Cannot determine the installation location. Trying to "
               << "uninstall from INTERNAL storage";
    return StepRemoveFiles::process();
  }
}

Step::Status StepRemoveFilesSD::undo() {
  LOG(DEBUG) << "RemoveFilesFromSD undo started";
    if (!installed_external_) {
      return StepRemoveFiles::undo();
    }
    LOG(DEBUG) << "Performing external installation";
    if (!storage_->PreInstallation()) {
      return StepRemoveFiles::undo();
      // TODO(a.niznik) Something is wrong here.
    }
    Step::Status status;
    status = StepRemoveFiles::undo();
    storage_->PostInstallation(true);
    return status;
  // TODO(a.niznik)
  // storage->PostUninstallation();
  // return StepRemoveFiles:: undo();
}

Step::Status StepRemoveFilesSD::clean() {
  LOG(DEBUG) << "RemoveFilesFromSD clean started";
  // if (installed_external_) {
  //  storage_->PostUninstallation();
  // }
  return StepRemoveFiles::clean();
}

StepRemoveFilesSD::Result StepRemoveFilesSD::CheckLocation() {
  int ret = app2ext_get_app_location(pkgid_.c_str());
  switch (ret) {
  case APP2EXT_SD_CARD:
    LOG(DEBUG) << "Application is installed on external storage";
    return Result::EXTERNAL;
  case APP2EXT_INTERNAL_MEM:
    LOG(DEBUG) << "Application is installed internal";
    return Result::INTERNAL;
  case APP2EXT_NOT_INSTALLED:
    LOG(ERROR) << "Application is not installed";
    return Result::ERROR;
  default:
    LOG(ERROR) << "Error in application installation";
    return Result::ERROR;
  }
}

}  // namespace filesystem
}  // namespace common_installer
