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
  installed_external_ = false;
  Step::Status status;
  Result res = CheckLocation();
  switch (res) {
  case Result::EXTERNAL: {
    LOG(DEBUG) << "Uninstallation from external storage";
    storage_ = std::unique_ptr<common_installer::WgtExternalStorage>(
          new common_installer::WgtExternalStorage(pkgid_));
    if (!storage_->IsInitialized()) {
      LOG(ERROR) << "Error in uninstallation";
      return StepRemoveFiles::process();
    }
    if (!storage_->PreUninstallation()) {
      LOG(DEBUG) << "Preuninstallation failed";
      status = StepRemoveFiles::process();
      storage_->PostUninstallation();
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
    LOG(ERROR) << "Error in uninstallation";
    return StepRemoveFiles::process();
  }
}

common_installer::Step::Status StepWgtRemoveFilesSD::undo() {
  LOG(DEBUG) << "WgtRemoveFilesFromSD undo started";
    if (!installed_external_) {
      return StepRemoveFiles::undo();
    }
    LOG(DEBUG) << "Performing external installation";
    if (!storage_->PreInstallation()) {
      return StepRemoveFiles::undo();
    }
    Step::Status status;
    status = StepRemoveFiles::undo();
    storage_->PostInstallation(true);
    return status;
  // TODO(a.niznik)
  // storage->PostUninstallation();
  // return StepRemoveFiles:: undo();
}

common_installer::Step::Status StepWgtRemoveFilesSD::clean() {
  LOG(DEBUG) << "WgtRemoveFilesFromSD clean started";
  if (installed_external_) {
    storage_->PostUninstallation();
  }
  return StepRemoveFiles::clean();
}

StepWgtRemoveFilesSD::Result StepWgtRemoveFilesSD::CheckLocation() {
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
