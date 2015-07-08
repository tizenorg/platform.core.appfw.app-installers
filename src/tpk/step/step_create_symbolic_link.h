/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#ifndef TPK_STEP_STEP_CREATE_SYMBOLIC_LINK_H_
#define TPK_STEP_STEP_CREATE_SYMBOLIC_LINK_H_

#include "common/app_installer.h"
#include "common/utils/logging.h"

namespace tpk {
namespace filesystem {

class StepCreateSymbolicLink : public common_installer::Step {
 public:
  using Step::Step;
  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override;
  Status precheck() override;

  SCOPE_LOG_TAG(SymbolicLink)
};

}  // namespace filesystem
}  // namespace tpk

#endif  // TPK_STEP_STEP_CREATE_SYMBOLIC_LINK_H_
