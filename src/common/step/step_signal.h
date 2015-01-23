/* 2014, Copyright © Eurogiciel Coporation, APACHE-2.0, see LICENSE file */

#ifndef COMMON_STEP_STEP_SIGNAL_H_
#define COMMON_STEP_STEP_SIGNAL_H_

#include <string>

#include "common/context_installer.h"

#include "common/step/step.h"

namespace common_installer {
namespace signal {

class StepSignal : public Step {
 public:
  int process(ContextInstaller* context) override;
  int clean(ContextInstaller* context) override;
  int undo(ContextInstaller* context) override;
 private:
  bool sendSignal(ContextInstaller* data, const std::string& key,
    const std::string& value);
};

}  // namespace signal
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_SIGNAL_H_
