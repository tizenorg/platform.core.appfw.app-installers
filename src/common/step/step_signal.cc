/* 2014, Copyright © Eurogiciel Coporation, APACHE-2.0, see LICENSE file */

#include "common/step/step_signal.h"

#include <pkgmgr-info.h>
#include <unistd.h>

#include "common/utils.h"

namespace common_installer {
namespace signal {

bool StepSignal::sendSignal(ContextInstaller* context, const std::string& key,
                              const std::string& value) {
    if (!context->pi()) {
        LOG(ERROR) << "PkgmgrSingal not yet intialized";
        return false;
    }

    if (key.empty() || value.empty()) {
        LOG(ERROR) << "key or value is empty";
        return false;
    }

    // send pkgmgr signal
    if (pkgmgr_installer_send_signal(
            context->pi(), context->manifest_data()->type,
            context->pkgid().c_str(),
            key.c_str(), value.c_str())) {
        LOG(ERROR) << "Fail to send pkgmgr signal";
        return false;
    }

    LOG(DEBUG) << "Success to send pkgmgr signal";
    return true;
}

Step::Status StepSignal::process() {
  sendSignal(context_, PKGMGR_INSTALLER_START_KEY_STR,
      PKGMGR_INSTALLER_INSTALL_EVENT_STR);
  LOG(INFO) << "Send Start";
  return Status::OK;
}

Step::Status StepSignal::clean() {
  sendSignal(context_, PKGMGR_INSTALLER_END_KEY_STR,
      PKGMGR_INSTALLER_OK_EVENT_STR);
  LOG(INFO) << "Send Success";
  return Status::OK;
}

Step::Status StepSignal::undo() {
  sendSignal(context_, PKGMGR_INSTALLER_END_KEY_STR,
      PKGMGR_INSTALLER_FAIL_EVENT_STR);
  LOG(ERROR) << "Send Error";
  return Status::OK;
}





}  // namespace signal
}  // namespace common_installer
