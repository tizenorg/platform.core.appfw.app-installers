// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include <app2ext_interface.h>

#include <string>

#include "common/wgt_external_storage.h"
#include "wgt/step/step_wgt_remove_files_sd.h"

namespace wgt {
namespace filesystem {

common_installer::Step::Status StepWgtRemoveFilesSD::precheck() {
  LOG(DEBUG) << "WgtRemoveFilesFromSD precheck started";
  pkgid_ = context_->pkgid.get().c_str();
  return StepRemoveFiles::precheck();
}

common_installer::Step::Status StepWgtRemoveFilesSD::process() {
  LOG(DEBUG) << "WgtRemoveFilesFromSD process started";
  Result res = Location();
  Step::Status status;
  switch (res) {
  case Result::EXTERNAL: {
    LOG(DEBUG) << "Uninstallation from external storage";
    ci::WgtExternalStorage* storage = new ci::WgtExternalStorage();
    if (storage->PreUninstallation(pkgid_)) {
      LOG(DEBUG) << "PreUninstallation executed succesfully-postuninstallation";
      status = StepRemoveFiles::process();
      storage->PostUninstallation(pkgid_);
    } else {
      LOG(DEBUG) << "PreUninstallation failed-normal uninstallation";
      status = StepRemoveFiles::process();
      storage->PostUninstallation(pkgid_);
    }
    storage->~WgtExternalStorage();
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
    Step::Status status;
    ci::WgtExternalStorage* storage = new ci::WgtExternalStorage();
    if (storage->PreInstallation(pkgid_)) {
      status = StepRemoveFiles::undo();
      storage->PostInstallation(APP2EXT_STATUS_SUCCESS, pkgid_);
    } else {
        status = StepRemoveFiles::undo();
        storage->PostInstallation(APP2EXT_STATUS_FAILED, pkgid_);
      }
    storage->~WgtExternalStorage();
    return status;
  } else {
      return StepRemoveFiles::undo();
  }
}

common_installer::Step::Status StepWgtRemoveFilesSD::clean() {
  LOG(DEBUG) << "WgtRemoveFilesFromSD clean started";
  return StepRemoveFiles::clean();
}

StepWgtRemoveFilesSD::Result StepWgtRemoveFilesSD::Location() {
  int ret = app2ext_get_app_location(pkgid_);
  LOG(DEBUG) << "ret: " << ret;
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
