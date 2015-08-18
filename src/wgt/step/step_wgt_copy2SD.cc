
#ifndef STEP_WGT_COPT2SD_CC
#define STEP_WGT_COPT2SD_CC

#include <app2ext_interface.h>
#include "wgt/step/step_wgt_copy2SD.h"

#include "common/step/step.h"
#include "common/step/step_copy.h"

#include <cassert>
#include <cstring>
#include <string>

#include "common/utils/file_util.h"
#include "wgt/wgt_backend_data.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace wgt {
namespace filesystem {

common_installer::Step::Status StepWgtCopy2SD::precheck() {
    LOG(DEBUG) << "Step precheck";
  return StepCopy::precheck();
}

common_installer::Step::Status StepWgtCopy2SD::process() {
    LOG(INFO) << "Step process";
     Step::Status status;
 backend_data_ = static_cast<WgtBackendData*>(context_->backend_data.get());
    LOG(INFO) << "backend_data " << backend_data_;
 if (backend_data_->settings.get().install_location() == wgt::parse::SettingInfo::InstallLocation::EXTERNAL) {
     LOG(INFO) << "Install location: EXTERNAL";
     LOG(INFO) << "PreInstallation result: " << PreInstallation();
     if (PreInstallation() == true) {
         status = StepCopy::process();
         LOG(INFO) << "Process executed succesfully, postinstallation:";
         PostInstallation(true);
     }
     else {
         LOG(INFO) << "PreInstallation failed, normal installation";
         status = StepCopy::process();
         LOG(INFO) << "Normal installation";
         return status;
     }
 }
 else {
     status = StepCopy::process();
     LOG(INFO) << "Normal installation";
 }
 return status;
}

common_installer::Step::Status StepWgtCopy2SD::undo() {
    LOG(INFO) << "Undo";
  return StepCopy::undo();
}

common_installer::Step::Status StepWgtCopy2SD::clean() {
    LOG(INFO) << "Clean";
  return StepCopy::clean();
}

bool StepWgtCopy2SD::PreInstallation() {
    LOG(INFO) << "Preinstallation";
    int ret = APP2EXT_SUCCESS;
    GList *dir_list = NULL;
    app2_handle = NULL;
    app2_handle = app2ext_init(APP2EXT_SD_CARD);
    LOG(INFO) << "app2_handle " << app2_handle;

    if (app2_handle == NULL)
        return false;
    else {
     LOG(INFO) << "ret value: " <<ret;
     ret = app2_handle->interface.pre_install(appname.c_str(), dir_list, size);
     if (ret != APP2EXT_SUCCESS) {
         LOG(INFO) << "APP2EXT_FAILURE in preinstallation "<<ret;
         g_list_free(dir_list);
         return false;
     }
    }
    LOG(INFO) << "APP2EXT_SUCCESS in preinstallation";
    g_list_free(dir_list);
    return true;
}

void StepWgtCopy2SD::PostInstallation(bool status) {
    LOG(INFO) << "Postinstallation";
    if (!status) {
        app2_handle->interface.post_install(appname.c_str(), APP2EXT_STATUS_FAILED);
        LOG(INFO) << "APP2EXT_FAILED in postinstallation";
        return;
    } else {
        app2_handle->interface.post_install(appname.c_str(), APP2EXT_STATUS_SUCCESS);
        LOG(INFO) << "APP2EXT_SUCCESS";
    }
    if (app2_handle != NULL) {
        if (app2ext_deinit(app2_handle) > 0) {
           // LOG(ERROR) << "App2ext deinitialization failed";
            LOG(INFO) << "App2ext deinitialization failed";
        }
    }
    LOG(INFO) << "App2ext deinitialization success";
}

}  //  namespace filesystem
}  //  namespace wgt

#endif //  STEP_WGT_COPT2SD_CC

