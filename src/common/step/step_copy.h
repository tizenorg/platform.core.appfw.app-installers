/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#ifndef COMMON_STEP_STEP_COPY_H_
#define COMMON_STEP_STEP_COPY_H_

#include "common/context_installer.h"

#include "common/step/step.h"

namespace common_installer {
namespace copy {

class StepCopy : public Step {
 public:
  using Step::Step;

  Step::Status process() override;
  Step::Status clean() override;
  Step::Status undo() override;
};

}  // namespace copy
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_COPY_H_
