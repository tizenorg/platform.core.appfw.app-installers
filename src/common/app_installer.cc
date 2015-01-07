/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include <iostream>
#include <cstdio>

#include "common/app_installer.h"
#include "common/context_installer.h"

namespace common_installer {

AppInstaller::AppInstaller(int request, const char* file, const char* pkgid) {
  context_.reset(new ContextInstaller());
  context_->set_request_type(request);
  context_->set_pkgid(pkgid);
  context_->set_file_path(file);
}

AppInstaller::~AppInstaller() {
}

int AppInstaller::Run() {
  std::list<std::unique_ptr<Step>>::iterator it(steps_.begin());
  std::list<std::unique_ptr<Step>>::iterator itStart(steps_.begin());
  std::list<std::unique_ptr<Step>>::iterator itEnd(steps_.end());

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
        break;
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
  return ret;
}

}  // namespace common_installer
