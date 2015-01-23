/* 2014, Copyright © Eurogiciel Coporation, APACHE-2.0, see LICENSE file */

#include "common/step/step_signal.h"

#include <pkgmgr-info.h>
#include <unistd.h>

#include <iostream>
#include "common/utils.h"

#define DBG(msg) std::cout << "[Signal] " << msg << std::endl;
#define ERR(msg) std::cout << "[ERROR: Signal] " << msg << std::endl;

namespace common_installer {
namespace signal {

bool StepSignal::sendSignal(ContextInstaller* data, const std::string& key,
                              const std::string& value) {
    if (!data->pi()) {
        ERR("PkgmgrSingal not yet intialized");
        return false;
    }

    if (key.empty() || value.empty()) {
        DBG("key or value is empty");
        return false;
    }

    // send pkgmgr signal
    if (pkgmgr_installer_send_signal(
            data->pi(), data->manifest_data()->type, data->pkgid().c_str(),
            key.c_str(), value.c_str())) {
        ERR("Fail to send pkgmgr signal");
        return false;
    }

    DBG("Success to send pkgmgr signal");
    return true;
}


int StepSignal::process(ContextInstaller* data) {
  sendSignal(data, PKGMGR_INSTALLER_START_KEY_STR,
    PKGMGR_INSTALLER_INSTALL_EVENT_STR);
  DBG("Send Start");
  return APPINST_R_OK;
}

int StepSignal::clean(ContextInstaller* data) {
  sendSignal(data, PKGMGR_INSTALLER_END_KEY_STR,
    PKGMGR_INSTALLER_OK_EVENT_STR);
  DBG("Send Sucess");
  return APPINST_R_OK;
}

int StepSignal::undo(ContextInstaller* data) {
  sendSignal(data, PKGMGR_INSTALLER_END_KEY_STR,
    PKGMGR_INSTALLER_FAIL_EVENT_STR);
  DBG("Send Error");
  return APPINST_R_OK;
}





}  // namespace signal
}  // namespace common_installer
