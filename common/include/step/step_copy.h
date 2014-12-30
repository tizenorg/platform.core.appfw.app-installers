/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#ifndef COMMON_INCLUDE_STEP_STEP_COPY_H_
#define COMMON_INCLUDE_STEP_STEP_COPY_H_

#include "include/context_installer.h"

#include <boost/filesystem.hpp>

#include "include/step/step.h"

using namespace boost::filesystem;

class StepCopy : public Step {
 public:
  path pkg_path_;

  int process(ContextInstaller* context) override;
  int clean(ContextInstaller* context) override;
  int undo(ContextInstaller* context) override;
};

#endif  // COMMON_INCLUDE_STEP_STEP_COPY_H_
