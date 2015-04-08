/* 2014, Copyright © Samsung, license APACHE-2.0, see LICENSE file */

#ifndef WGT_STEP_STEP_CREATE_SYMBOLIC_LINK_H_
#define WGT_STEP_STEP_CREATE_SYMBOLIC_LINK_H_

#include <boost/filesystem.hpp>

#include "common/app_installer.h"
#include "common/context_installer.h"
#include "common/step/step.h"
#include "utils/logging.h"

namespace wgt {
namespace symbolic_link {

class StepCreateSymbolicLink : public common_installer::Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override;
  Status undo() override;
  Status precheck() override { return Status::OK; }

  SCOPE_LOG_TAG(SymbolicLink)
};

}  // namespace symbolic_link
}  // namespace wgt

#endif  // WGT_STEP_STEP_CREATE_SYMBOLIC_LINK_H_
