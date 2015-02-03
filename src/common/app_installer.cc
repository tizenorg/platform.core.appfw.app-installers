/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include <iostream>
#include <cstdio>

#include "common/app_installer.h"
#include "common/context_installer.h"
#include "common/pkgmgr_signal.h"

#define STR_EMPTY ""

namespace common_installer {

AppInstaller::AppInstaller(pkgmgr_installer *pi, const char* package_type)
  : context_(new ContextInstaller()) {
  int request_type = pkgmgr_installer_get_request_type(pi);
  context_->set_pi(std::unique_ptr<PkgmgrSignal>(new PkgmgrSignal(pi)));
  context_->set_request_type(request_type);
  context_->set_pkg_type(package_type);
  switch (request_type) {
    case PKGMGR_REQ_INSTALL:
     context_->set_file_path(pkgmgr_installer_get_request_info(pi));
     context_->set_new_temporary_pkgid();
    break;
    case PKGMGR_REQ_UNINSTALL:
     context_->set_pkgid(pkgmgr_installer_get_request_info(pi));
     context_->set_file_path(STR_EMPTY);
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

  sendSignal(PKGMGR_INSTALLER_START_KEY_STR,
             getEventStr(context_->request_type()));

  int ret = 0;
  for (; it != itEnd; ++it) {
    if ((*it)->process() != Step::Status::OK) {
      std::cout << "Error during processing" << std::endl;
      ret = -1;
      break;
    }
  }
  if (it != itEnd) {
    std::cout << "Failure occurs" << std::endl;
    do {
      if ((*it)->undo() != Step::Status::OK) {
        std::cout << "Error during undo operation" << std::endl;
        ret = -2;
      }
    } while (it-- != itStart);
  } else {
    while (it-- != itStart) {
      if ((*it)->clean() != Step::Status::OK) {
        std::cout << "Error during clean operation" << std::endl;
        ret = -3;
        break;
      }
    }
  }
  if (0 != ret) {
    sendSignal(PKGMGR_INSTALLER_END_KEY_STR, PKGMGR_INSTALLER_FAIL_EVENT_STR);
  } else {
    sendSignal(PKGMGR_INSTALLER_END_KEY_STR, PKGMGR_INSTALLER_OK_EVENT_STR);
  }

  EnsureSignalSend();

  return ret;
}


const char *
AppInstaller::getEventStr(const int request) {
  switch (request) {
    case PKGMGR_REQ_INSTALL:
      return PKGMGR_INSTALLER_INSTALL_EVENT_STR;
    case PKGMGR_REQ_UNINSTALL:
      return PKGMGR_INSTALLER_UNINSTALL_EVENT_STR;
    case PKGMGR_REQ_MOVE:
      return PKGMGR_INSTALLER_MOVE_EVENT_STR;
    case PKGMGR_REQ_UPGRADE:
      return PKGMGR_INSTALLER_UPGRADE_EVENT_STR;
    default:
      return PKGMGR_INSTALLER_FAIL_EVENT_STR;
  }
}


void
AppInstaller::sendSignal(const char* signal_key, const char* val) {
  pkgmgr_installer_send_signal(context_->pi(),
                               context_->pkg_type().c_str(),
                               context_->pkgid().c_str(),
                               signal_key, val);
}
}  // namespace common_installer
