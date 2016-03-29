/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#ifndef COMMON_STEP_PKGMGR_STEP_REGISTER_APP_H_
#define COMMON_STEP_PKGMGR_STEP_REGISTER_APP_H_

#include <manifest_parser/utils/logging.h>

#include "common/installer_context.h"
#include "common/step/step.h"

namespace common_installer {
namespace pkgmgr {

class StepRegisterApplication : public Step {
 public:
  using Step::Step;

  __attribute__ ((visibility ("default"))) Status process() override;
  __attribute__ ((visibility ("default"))) Status clean() override { return Status::OK; }
  __attribute__ ((visibility ("default"))) Status undo() override;
  __attribute__ ((visibility ("default"))) Status precheck() override;

  SCOPE_LOG_TAG(RegisterApp)
};

}  // namespace pkgmgr
}  // namespace common_installer


#endif  // COMMON_STEP_PKGMGR_STEP_REGISTER_APP_H_
