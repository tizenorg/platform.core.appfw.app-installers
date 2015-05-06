/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#ifndef COMMON_STEP_STEP_REGISTER_APP_H_
#define COMMON_STEP_STEP_REGISTER_APP_H_

#include "common/context_installer.h"
#include "common/step/step.h"
#include "utils/logging.h"

namespace common_installer {
namespace register_app {

class StepRegisterApplication : public Step {
 public:
  explicit StepRegisterApplication(ContextInstaller* context)
      : Step(context),
        in_registry_(false) { }

  Status process() override;
  Status clean() override;
  Status undo() override;
  Status precheck() override;

 private:
  bool in_registry_;

  SCOPE_LOG_TAG(RegisterApp)
};

}  // namespace register_app
}  // namespace common_installer


#endif  // COMMON_STEP_STEP_REGISTER_APP_H_
