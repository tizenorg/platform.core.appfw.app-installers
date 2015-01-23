/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include <iostream>
#include <cstdio>

#include "common/app_installer.h"
#include "common/context_installer.h"

namespace common_installer {

AppInstaller::AppInstaller(pkgmgr_installer *pi) {
  ctx_ = new ContextInstaller();
  int request_type = pkgmgr_installer_get_request_type(pi);
  ctx_->set_pi(pi);
  ctx_->set_request_type(request_type);
  switch (request_type) {
    case PKGMGR_REQ_INSTALL:
     ctx_->set_file_path(pkgmgr_installer_get_request_info(pi));
     ctx_->set_pkgid("");
    break;
    case PKGMGR_REQ_UNINSTALL:
     ctx_->set_pkgid(pkgmgr_installer_get_request_info(pi));
     ctx_->set_file_path("");
    break;
  }
}

AppInstaller::~AppInstaller() { delete ctx_; }


int AppInstaller::AddStep(Step *step) {
  try {
    ListStep.push_back(step);
  } catch (int e) {
    return -1;
  }
  return 0;
}


int AppInstaller::Run() {
  std::list<Step *>::iterator it(ListStep.begin());
  std::list<Step *>::iterator itStart(ListStep.begin());
  std::list<Step *>::iterator itEnd(ListStep.end());

  int ret = 0;
  for (; it != itEnd; ++it) {
    if ((*it)->process(ctx_) != APPINST_R_OK) {
      std::cout << "Error during processing" << std::endl;
      ret = -1;
      break;
    }
  }
  if (it != itEnd) {
    std::cout << "Failure occurs" << std::endl;
    do {
      if ((*it)->undo(ctx_) != APPINST_R_OK) {
        std::cout << "Error during undo operation" << std::endl;
        ret = -2;
      }
    } while (it-- != itStart);
  } else {
    while (it-- != itStart) {
      if ((*it)->clean(ctx_) != APPINST_R_OK) {
        std::cout << "Error during clean operation" << std::endl;
        ret = -3;
        break;
      }
    }
  }
  return ret;
}

}  // namespace common_installer
