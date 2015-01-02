/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#ifndef COMMON_INCLUDE_APP_INSTALLER_H_
#define COMMON_INCLUDE_APP_INSTALLER_H_

#include <list>

#include "include/step/step.h"

namespace common_installer {

class AppInstaller {
 private:
  std::list<Step*> ListStep;
  ContextInstaller* ctx_;

 public:
  AppInstaller(int request, const char* file, const char* pkgid);
  ~AppInstaller();

  int AddStep(Step* step);
  int Run();
};

}  // namespace common_installer

#endif  // COMMON_INCLUDE_APP_INSTALLER_H_
