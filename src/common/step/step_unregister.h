/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#ifndef COMMON_STEP_STEP_UNREGISTER_H_
#define COMMON_STEP_STEP_UNREGISTER_H_

#include "common/context_installer.h"
#include "common/step/step.h"

//TODO do we want to put these uninstall steps into separate directory?
//maybe not, as they might be used for instance during update

namespace common_installer {
namespace unregister {

class StepUnregister : public Step {
 public:
  int process(ContextInstaller* context) override;
  int clean(ContextInstaller* context) override;
  int undo(ContextInstaller* context) override;
};

}  // namespace unregister
}  // namespace common_installer


#endif  // COMMON_STEP_STEP_UNREGISTER_H_
