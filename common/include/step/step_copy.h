/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#ifndef COMMON_INCLUDE_STEP_STEP_COPY_H_
#define COMMON_INCLUDE_STEP_STEP_COPY_H_

#include "include/context_installer.h"

#include "include/step/step.h"

class StepCopy : public Step {
 public:
  int process(ContextInstaller* context) override;
  int clean(ContextInstaller* context) override;
  int undo(ContextInstaller* context) override;
};

#endif  // COMMON_INCLUDE_STEP_STEP_COPY_H_
