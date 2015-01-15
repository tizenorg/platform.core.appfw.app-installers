/* 2014, Copyright © Eurogiciel Coporation, APACHE-2.0, see LICENSE file */

#ifndef COMMON_STEP_STEP_PARSE_H_
#define COMMON_STEP_STEP_PARSE_H_

#include "common/context_installer.h"

#include "common/step/step.h"

namespace common_installer {
namespace parse {

class StepParse : public Step {
 public:
  int process(ContextInstaller* context) override;
  int clean(ContextInstaller* context) override;
  int undo(ContextInstaller* context) override;
};

}  // namespace parse
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_PARSE_H_
