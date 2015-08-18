// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_wgt_copy_2sd.h"

#include <app2ext_interface.h>

#include <string>

#include "common/step/step.h"
#include "common/step/step_copy.h"
#include "common/utils/file_util.h"
#include "wgt/wgt_backend_data.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace wgt {
namespace filesystem {

common_installer::Step::Status StepWgtCopy2SD::precheck() {
  LOG(DEBUG) << "Step precheck";
  backend_data_ = static_cast<WgtBackendData*>(context_->backend_data.get());
  if (!backend_data_) {
      LOG(ERROR) << "No backend data";
      return common_installer::Step::Status::ERROR;
  }
  return StepCopy::precheck();
}

common_installer::Step::Status StepWgtCopy2SD::process() {
  LOG(INFO) << "Step process";
  std::string appname = " ";
  Step::Status status;
  if (External(backend_data_->settings.get().install_location())) {
      LOG(INFO) << "Install location: EXTERNAL";
      if (PreInstallation(appname)) {
          status = StepCopy::process();
          LOG(INFO) << "Process executed succesfully, postinstallation:";
          PostInstallation(true, appname);
          return status;
      } else {
          LOG(INFO) << "PreInstallation failed, normal installation";
          PostInstallation(false, appname);
          return StepCopy::process();
      }
  } else {
      LOG(INFO) << "Normal installation";
      return StepCopy::process();
  }
}

common_installer::Step::Status StepWgtCopy2SD::undo() {
  LOG(INFO) << "Undo";
  return StepCopy::undo();
  // TODO(a.niznik) this should be checked.
}

common_installer::Step::Status StepWgtCopy2SD::clean() {
  LOG(INFO) << "Clean";
  return StepCopy::clean();
}

bool StepWgtCopy2SD::External(parse::SettingInfo::InstallLocation location) {
  if (location == parse::SettingInfo::InstallLocation::EXTERNAL)
      return true;
  return false;
}

bool StepWgtCopy2SD::PreInstallation(std::string appname) {
  LOG(INFO) << "PreInstallation";
  int ret = APP2EXT_SUCCESS;
  int size = 0;
  GList *dir_list = NULL;
  app2_handle_ = app2ext_init(APP2EXT_SD_CARD);
  if (!app2_handle_) {
     return false;
  } else {
     LOG(INFO) << "ret value: " << ret;
     ret = app2_handle_->interface.pre_install(appname.c_str(), dir_list, size);
     if (ret != APP2EXT_SUCCESS) {
         LOG(INFO) << "APP2EXT_FAILURE in preinstallation " << ret;
         return false;
      }
    }
  LOG(INFO) << "APP2EXT_SUCCESS in preinstallation";
  g_list_free(dir_list);
  return true;
}

void StepWgtCopy2SD::PostInstallation(bool status, std::string appname) {
  LOG(INFO) << "PostInstallation";
  if (!status) {
  app2_handle_->interface.post_install(appname.c_str(), APP2EXT_STATUS_FAILED);
  LOG(INFO) << "APP2EXT_FAILED in postinstallation";
  return;
  } else {
  app2_handle_->interface.post_install(appname.c_str(), APP2EXT_STATUS_SUCCESS);
  LOG(INFO) << "APP2EXT_SUCCESS";
  }
  if (app2_handle_ != NULL) {
    if (app2ext_deinit(app2_handle_) > 0) {
    LOG(INFO) << "App2ext deinitialization failed";
      }
  }
  LOG(INFO) << "App2ext deinitialization success";
}

}  //  namespace filesystem
}  //  namespace wgt


