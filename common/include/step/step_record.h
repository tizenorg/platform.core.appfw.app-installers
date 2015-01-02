/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#ifndef COMMON_INCLUDE_STEP_STEP_RECORD_H_
#define COMMON_INCLUDE_STEP_STEP_RECORD_H_

#include "include/context_installer.h"

#include <boost/filesystem.hpp>

#include "include/step/step.h"

namespace common_installer {
namespace record {

namespace fs = boost::filesystem;

class StepRecord : public Step {
 private:
  fs::path xml_path_;
 public:
  int process(ContextInstaller* context) override;
  int clean(ContextInstaller* context) override;
  int undo(ContextInstaller* context) override;
};

}  // namespace record
}  // namespace common_installer

#endif  // COMMON_INCLUDE_STEP_STEP_RECORD_H_
