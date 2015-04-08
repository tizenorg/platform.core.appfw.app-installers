/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#ifndef COMMON_STEP_STEP_COPY_H_
#define COMMON_STEP_STEP_COPY_H_

#include "common/context_installer.h"

#include "common/step/step.h"
#include "utils/logging.h"

namespace common_installer {
namespace copy {

class StepCopy : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override;
  Status undo() override;
  Status precheck() override;

  SCOPE_LOG_TAG(Copy)
};

}  // namespace copy
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_COPY_H_
