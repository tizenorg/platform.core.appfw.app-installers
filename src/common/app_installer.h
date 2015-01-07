/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#ifndef COMMON_APP_INSTALLER_H_
#define COMMON_APP_INSTALLER_H_

#include <list>
#include <memory>

#include "common/step/step.h"

namespace common_installer {

class AppInstaller {
 public:
  AppInstaller(int request, const char* file, const char* pkgid);
  virtual ~AppInstaller();

  void AddStep(std::unique_ptr<Step> step);
  int Run();

 protected:
  std::unique_ptr<ContextInstaller> context_;

 private:
  AppInstaller(const AppInstaller& /*other*/) = delete;
  AppInstaller& operator=(const AppInstaller& /*other*/) = delete;

  std::list<std::unique_ptr<Step>> steps_;
};

}  // namespace common_installer

#endif  // COMMON_APP_INSTALLER_H_
