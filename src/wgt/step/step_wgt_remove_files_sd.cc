// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_wgt_remove_files_sd.h"

namespace wgt {
namespace filesystem {

common_installer::Step::Status StepWgtRemoveFilesSD::precheck() {
  LOG(DEBUG) << "WgtRemoveFilesFromSD precheck started";
  return StepRemoveFiles::precheck();
}

common_installer::Step::Status StepWgtRemoveFilesSD::process() {
  LOG(DEBUG) << "WgtRemoveFilesFromSD process started";
  pkgid_ = context_->pkgid.get();
  initialized_ = false;
  Step::Status status;
  Result res = CalculateLocation();
  switch (res) {
  case Result::EXTERNAL: {
    LOG(DEBUG) << "Uninstallation from external storage";
    storage_ = std::unique_ptr<common_installer::WgtExternalStorage>(
          new common_installer::WgtExternalStorage(pkgid_));
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
      initialized_ = true;
      }
  }
    return status;
  case Result::INTERNAL:
    LOG(DEBUG) << "Normal uninstallation";
    return StepRemoveFiles::process();
  default:
    LOG(ERROR) << "Error in uninstallation";
    return StepRemoveFiles::process();
  }
}

common_installer::Step::Status StepWgtRemoveFilesSD::undo() {
  LOG(DEBUG) << "WgtRemoveFilesFromSD undo started";
  if (CalculateLocation() == Result::EXTERNAL) {
    LOG(DEBUG) << "Performing external installation";
    if (!initialized_) {
      return Step::Status::ERROR;
    }
    Step::Status status;
    if (!storage_->PreInstallation()) {
      return Step::Status::ERROR;
    }
    status = StepRemoveFiles::undo();
    storage_->PostInstallation(true);
    return status;
  } else {
    return StepRemoveFiles::undo();
    }
}

common_installer::Step::Status StepWgtRemoveFilesSD::clean() {
  LOG(DEBUG) << "WgtRemoveFilesFromSD clean started";
  return StepRemoveFiles::clean();
}

StepWgtRemoveFilesSD::Result StepWgtRemoveFilesSD::CalculateLocation() {
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
}  // namespace wgt
