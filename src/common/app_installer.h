/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#ifndef COMMON_APP_INSTALLER_H_
#define COMMON_APP_INSTALLER_H_

#include <pkgmgr_installer.h>
#include <list>
#include <memory>

#include "common/step/step.h"
#include "utils/logging.h"
#include "utils/macros.h"

namespace common_installer {

class AppInstaller {
 public:
  explicit AppInstaller(pkgmgr_installer *pi);
  virtual ~AppInstaller();

  // Adds new step to installer by specified type
  // Type of template parameter is used to create requested step class instance.
  // Context of installer is passed to step in this method
  // and is not being exposed outside installer.
  template<class StepT>
  void AddStep() {
    steps_.push_back(std::unique_ptr<Step>(new StepT(context_.get())));
  }

  int Run();

 protected:
  std::unique_ptr<ContextInstaller> context_;

 private:
  std::list<std::unique_ptr<Step>> steps_;

  SCOPE_LOG_TAG(AppInstaller)

  DISALLOW_COPY_AND_ASSIGN(AppInstaller);
};

}  // namespace common_installer

#endif  // COMMON_APP_INSTALLER_H_
