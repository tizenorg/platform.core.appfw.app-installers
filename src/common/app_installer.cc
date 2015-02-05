/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include <cstdio>

#include "common/app_installer.h"
#include "common/context_installer.h"
#include "common/pkgmgr_signal.h"
#include "utils/logging.h"

namespace common_installer {

AppInstaller::AppInstaller(pkgmgr_installer *pi)
    : context_(new ContextInstaller()) {
  int request_type = pkgmgr_installer_get_request_type(pi);
  context_->set_pi(std::unique_ptr<PkgmgrSignal>(new PkgmgrSignal(pi)));
  context_->set_request_type(request_type);
  switch (request_type) {
    case PKGMGR_REQ_INSTALL:
     context_->set_file_path(pkgmgr_installer_get_request_info(pi));
     context_->set_pkgid("");
    break;
    case PKGMGR_REQ_UNINSTALL:
     context_->set_pkgid(pkgmgr_installer_get_request_info(pi));
     context_->set_file_path("");
    break;
  }
}

AppInstaller::~AppInstaller() {
}

void AppInstaller::EnsureSignalSend() {
  if (!context_->pi()->IsFinished()) {
    // if signal was not sent during normal step execution
    // then this will sent any signal about failure
    (void) context_->pi()->sendStarted();
    (void) context_->pi()->sendFinished(PkgmgrSignal::Result::FAILED);
  }
}

int AppInstaller::Run() {
  std::list<std::unique_ptr<Step>>::iterator it(steps_.begin());
  std::list<std::unique_ptr<Step>>::iterator itStart(steps_.begin());
  std::list<std::unique_ptr<Step>>::iterator itEnd(steps_.end());

  int ret = 0;
  for (; it != itEnd; ++it) {
    if ((*it)->process() != Step::Status::OK) {
      LOG(ERROR) << "Error during processing";
      ret = -1;
      break;
    }
  }
  if (it != itEnd) {
    LOG(ERROR) << "Failure occurs";
    do {
      if ((*it)->undo() != Step::Status::OK) {
        LOG(ERROR) << "Error during undo operation";
        ret = -2;
      }
    } while (it-- != itStart);
  } else {
    while (it-- != itStart) {
      if ((*it)->clean() != Step::Status::OK) {
        LOG(ERROR) << "Error during clean operation";
        ret = -3;
        break;
      }
    }
  }

  EnsureSignalSend();

  return ret;
}

}  // namespace common_installer
