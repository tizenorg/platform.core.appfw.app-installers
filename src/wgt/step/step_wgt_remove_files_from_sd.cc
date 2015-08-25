// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.


#include "wgt/step/step_wgt_remove_files_from_sd.h"

#include <app2ext_interface.h>
#include <pkgmgr-info.h>
#include <pkgmgrinfo_type.h>

#include <string>

namespace wgt {
namespace filesystem {

namespace p = parse;

common_installer::Step::Status StepWgtRemoveFilesFromSD::precheck() {
  LOG(DEBUG) << "WgtRemoveFilesFromSD precheck started";
  return StepRemoveFiles::precheck();
}

common_installer::Step::Status StepWgtRemoveFilesFromSD::process() {
  LOG(DEBUG) << "WgtRemoveFilesFromSD process started";
  std::string ppkgid_ = context_->pkgid.get();
  pkgid_ = ppkgid_.c_str();
  IsInstalledExternal(pkgid_);
  LOG(DEBUG) << "Storage: " << storage_;
  if (storage_ == PMINFO_EXTERNAL_STORAGE) {
    Step::Status status;
    LOG(DEBUG) << "Uninstallation from external storage";
    if (PreUninstallation()) {
      LOG(DEBUG) << "PreUninstallation executed succesfully-postuninstallation";
      status = StepRemoveFiles::process();
      PostUninstallation();
      Deinitialization();
      return status;
    } else {
      LOG(DEBUG) << "PreUninstallation failed-normal uninstallation";
      status = StepRemoveFiles::process();
      PostUninstallation();
      Deinitialization();
      return status;
    }
  } else {
    LOG(DEBUG) << "Normal uninstallation";
    return StepRemoveFiles::process();
  }
}

common_installer::Step::Status StepWgtRemoveFilesFromSD::undo() {
  LOG(DEBUG) << "WgtRemoveFilesFromSD undo started";
  return StepRemoveFiles::undo();
}

common_installer::Step::Status StepWgtRemoveFilesFromSD::clean() {
  LOG(DEBUG) << "WgtRemoveFilesFromSD clean started";
  return StepRemoveFiles::clean();
}

bool StepWgtRemoveFilesFromSD::PreUninstallation() {
  LOG(DEBUG) << "WgtRemoveFilesFromSD preuninstallation started";
  app2_handle_ = app2ext_init(APP2EXT_SD_CARD);
  if (!app2_handle_) {
    LOG(ERROR) << "App2Ext initialization failed";
    return false;
  }
  LOG(DEBUG) << "App2Ext initialization success";
  int ret = app2_handle_->interface.pre_uninstall(pkgid_);
  if (ret != APP2EXT_SUCCESS) {
    LOG(ERROR) << "App2Ext preuninstallation failed";
    return false;
  }
  LOG(DEBUG) << "App2Ext preuninstallation success";
  return true;
}

void StepWgtRemoveFilesFromSD::Deinitialization() {
  LOG(DEBUG) << "WgtRemoveFilesFromSD deinitialization started";
  if (app2ext_deinit(app2_handle_) != APP2EXT_SUCCESS) {
    LOG(ERROR) << "App2Ext deinitialization failed";
    return;
  }
  LOG(DEBUG) << "App2Ext deinitialization success";
}

void StepWgtRemoveFilesFromSD::PostUninstallation() {
  LOG(DEBUG) << "WgtRemoveFilesFromSD postuninstallation started";
  if (app2_handle_->interface.post_uninstall(pkgid_) != APP2EXT_SUCCESS) {
    LOG(ERROR) << "App2Ext postuninstallation failed";
    return;
  }
  LOG(DEBUG) << "App2Ext postuninstallation success";
}

void StepWgtRemoveFilesFromSD::IsInstalledExternal(const char* pkgid_) {
  pkgmgrinfo_pkginfo_h handle = nullptr;
  int ret = pkgmgrinfo_pkginfo_get_pkginfo(pkgid_, &handle);
  LOG(DEBUG) << "pkgid: " << pkgid_;
  if (ret != PMINFO_R_OK) {
    LOG(ERROR) << "pkgmgrinfo_pkginfo_get_pkginfo failed with error:" << ret;
    return;
  }
  ret = pkgmgrinfo_pkginfo_get_installed_storage(handle, &storage_);
  if (ret != PMINFO_R_OK) {
    pkgmgrinfo_pkginfo_destroy_pkginfo(handle);
    LOG(ERROR) << "pkgmgrinfo_pkginfo_get_installed_storage failed with error:"
               << ret;
    return;
  }
  pkgmgrinfo_pkginfo_destroy_pkginfo(handle);
}

}  // namespace filesystem
}  // namespace wgt
