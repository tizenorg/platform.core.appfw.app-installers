/* 2014, Copyright © Eurogiciel Coporation, APACHE-2.0, see LICENSE file */

#ifndef COMMON_STEP_STEP_SIGNAL_H_
#define COMMON_STEP_STEP_SIGNAL_H_

#include <string>

#include "common/context_installer.h"

#include "common/step/step.h"
#include "utils/logging.h"

namespace common_installer {
namespace signal {

class StepSignal : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override;
  Status undo() override;
 private:
  SCOPE_LOG_TAG(Signal)
};

}  // namespace signal
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_SIGNAL_H_
