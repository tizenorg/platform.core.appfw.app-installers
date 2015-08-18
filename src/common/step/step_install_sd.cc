// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_install_sd.h"

namespace common_installer {
namespace filesystem {

common_installer::Step::Status StepInstallSD::precheck() {
  LOG(DEBUG) << "StepInstallSD precheck started";
  if (!context_->manifest_data.get()) {
    LOG(ERROR) << "No manifest data";
    return Step::Status::ERROR;
  }
  return Step::Status::OK;
}

common_installer::Step::Status StepInstallSD::process() {
  LOG(DEBUG) << "StepInstallSD process started";
  if (strcmp(context_->manifest_data.get()->installlocation,
             "internal-only") == 0)
    return Step::Status::OK;
  storage_ = std::unique_ptr<ExternalStorage>(
       new ExternalStorage(context_->pkgid.get()));
  //  external installation
  if (storage_->PostInstallation(context_->preinstall.get()))
    return Step::Status::OK;
  else
    return Step::Status::ERROR;
}

common_installer::Step::Status StepInstallSD::undo() {
  LOG(DEBUG) << "StepInstallSD undo started";
  if (strcmp(context_->manifest_data.get()->installlocation,
             "internal-only") == 0)
    return Step::Status::OK;
  storage_->PreUninstallation();
  storage_->PostUninstallation();
  return Step::Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer
