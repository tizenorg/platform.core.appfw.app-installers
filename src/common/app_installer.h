/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#ifndef COMMON_APP_INSTALLER_H_
#define COMMON_APP_INSTALLER_H_

#include <pkgmgr_installer.h>
#include <list>

#include "common/step/step.h"


namespace common_installer {

class AppInstaller {
 private:
  std::list<Step*> ListStep;
  ContextInstaller* ctx_;

 public:
  AppInstaller(pkgmgr_installer *pi);
  ~AppInstaller();

  int AddStep(Step* step);
  int Run();
};

}  // namespace common_installer

#endif  // COMMON_APP_INSTALLER_H_
