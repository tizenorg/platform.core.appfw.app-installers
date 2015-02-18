/* 2014, Copyright © Eurogiciel Coporation, APACHE-2.0, see LICENSE file */

#ifndef COMMON_STEP_STEP_PARSE_H_
#define COMMON_STEP_STEP_PARSE_H_

#include "common/context_installer.h"

#include "common/step/step.h"
#include "utils/logging.h"

namespace common_installer {
namespace parse {

class StepParse : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override;
  Status undo() override;

  SCOPE_LOG_TAG(Parse)
};

}  // namespace parse
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_PARSE_H_
