// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_wgt_remove_files_sd.h"

namespace wgt {
namespace filesystem {

common_installer::Step::Status StepWgtRemoveFilesSD::precheck() {
  LOG(DEBUG) << "WgtRemoveFilesFromSD precheck started";
  pkgid_ = context_->pkgid.get();
  return StepRemoveFiles::precheck();
}

common_installer::Step::Status StepWgtRemoveFilesSD::process() {
  LOG(DEBUG) << "WgtRemoveFilesFromSD process started";
  Result res = Location();
  Step::Status status;
  switch (res) {
  case Result::EXTERNAL: {
    LOG(DEBUG) << "Uninstallation from external storage";
    storage_ = new ci::WgtExternalStorage();
    if (!storage_->IsInitialized()) {
      LOG(ERROR) << "Error in uninstallation";
      return StepRemoveFiles::process();
    }
    if (!storage_->PreUninstallation(pkgid_)) {
      LOG(DEBUG) << "Preuninstallation failed";
      status = StepRemoveFiles::process();
      storage_->PostUninstallation(pkgid_);
    } else {
      LOG(DEBUG) << "PreUninstallation failed-normal uninstallation";
      status = StepRemoveFiles::process();
      storage_->PostUninstallation(pkgid_);
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
  if (Location() == Result::EXTERNAL) {
    LOG(DEBUG) << "EXTERNAL";
    Step::Status status;
    if (storage_->PreInstallation(pkgid_)) {
      status = StepRemoveFiles::undo();
      storage_->PostInstallation(true, pkgid_);
    } else {
        status = StepRemoveFiles::undo();
        storage_->PostInstallation(false, pkgid_);
      }
    delete storage_;
    return status;
  } else {
    return StepRemoveFiles::undo();
  }
}

common_installer::Step::Status StepWgtRemoveFilesSD::clean() {
  LOG(DEBUG) << "WgtRemoveFilesFromSD clean started";
  if (storage_) {
  delete storage_;
  }
  return StepRemoveFiles::clean();
}

StepWgtRemoveFilesSD::Result StepWgtRemoveFilesSD::Location() {
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
