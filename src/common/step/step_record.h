/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#ifndef COMMON_STEP_STEP_RECORD_H_
#define COMMON_STEP_STEP_RECORD_H_

#include "common/context_installer.h"
#include "common/step/step.h"
#include "utils/logging.h"

namespace common_installer {
namespace record {

class StepRecord : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override;
  Status undo() override;

  SCOPE_LOG_TAG(Record)
};

}  // namespace record
}  // namespace common_installer


#endif  // COMMON_STEP_STEP_RECORD_H_
