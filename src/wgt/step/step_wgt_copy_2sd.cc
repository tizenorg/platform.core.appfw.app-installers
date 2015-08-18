// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_wgt_copy_2sd.h"

#include <app2ext_interface.h>

#include <string>

namespace wgt {
namespace filesystem {

common_installer::Step::Status StepWgtCopy2SD::precheck() {
  LOG(DEBUG) << "WgtCopy2SD precheck started";
  backend_data_ = static_cast<WgtBackendData*>(context_->backend_data.get());
  if (!backend_data_) {
    LOG(ERROR) << "No backend data";
    return common_installer::Step::Status::ERROR;
  }
  return StepCopy::precheck();
}

common_installer::Step::Status StepWgtCopy2SD::process() {
  LOG(DEBUG) << "WgtCopy2SD process started";
  if (External(backend_data_->settings.get().install_location())) {
    const std::string& appname = "";
    Step::Status status;
    LOG(DEBUG) << "Install location: EXTERNAL";
    if (PreInstallation(appname)) {
      LOG(DEBUG) << "Preinstallation executed succesfully, postinstallation";
      status = StepCopy::process();
      PostInstallation(true, appname);
      return status;
    } else {
        LOG(ERROR) << "PreInstallation failed, normal installation";
        status = StepCopy::process();
        PostInstallation(false, appname);
        return status;
    }
  } else {
      LOG(DEBUG) << "Normal installation";
      return StepCopy::process();
  }
}

common_installer::Step::Status StepWgtCopy2SD::undo() {
  LOG(DEBUG) << "WgtCopy2SD undo started";
  return StepCopy::undo();
  // TODO(a.niznik) this should be fixed. Rollback will not work.
}

common_installer::Step::Status StepWgtCopy2SD::clean() {
  LOG(DEBUG) << "WgtCopy2SD clean started";
  return StepCopy::clean();
}

bool StepWgtCopy2SD::External(parse::SettingInfo::InstallLocation location) {
  return location == parse::SettingInfo::InstallLocation::EXTERNAL;
}

bool StepWgtCopy2SD::PreInstallation(const std::string& appname) {
  LOG(DEBUG) << "WgtCopy2SD preInstallation started";
  int ret = APP2EXT_SUCCESS;
  int size = 0;
  GList *dir_list = nullptr;
  app2_handle_ = app2ext_init(APP2EXT_SD_CARD);
  if (!app2_handle_) {
    return false;
  }
  LOG(DEBUG) << "App2Ext initialization failed with error code: " << ret;
  ret = app2_handle_->interface.pre_install(appname.c_str(), dir_list, size);
  if (ret != APP2EXT_SUCCESS) {
    LOG(ERROR) << "App2Ext preinstallation failed";
    return false;
    }
  LOG(DEBUG) << "App2Ext preinstallation success";
  g_list_free(dir_list);
  return true;
}

void StepWgtCopy2SD::Deinitialization(app2ext_handle* app2_handle_) {
  LOG(DEBUG) << "WgtCopy2SD deinitialization";
  if (app2ext_deinit(app2_handle_) != APP2EXT_SUCCESS) {
    LOG(ERROR) << "App2Ext deinitialization failed";
    return;
  }
  LOG(DEBUG) << "App2Ext deinitialization success";
}

void StepWgtCopy2SD::PostInstallation(bool status, const std::string& app) {
  LOG(DEBUG) << "WgtCopy2SD postInstallation started";
  if (!status) {
    app2_handle_->interface.post_install(app.c_str(), APP2EXT_STATUS_FAILED);
    LOG(DEBUG) << "APP2EXT_STATUS_FAILED";
    if (app2_handle_) {
      Deinitialization(app2_handle_);
      return;
    } else {
        LOG(ERROR) << "App2Ext postInstallation failed";
        return;
    }
  }
  app2_handle_->interface.post_install(app.c_str(), APP2EXT_STATUS_SUCCESS);
  LOG(DEBUG) << "APP2EXT_STATUS_SUCCESS";
  if (app2_handle_) {
    Deinitialization(app2_handle_);
  } else {
  LOG(ERROR) << "App2Ext postInstallation failed";
  return;
  }
  LOG(DEBUG) << "App2Ext postInstallation success";
}

}  //  namespace filesystem
}  //  namespace wgt


