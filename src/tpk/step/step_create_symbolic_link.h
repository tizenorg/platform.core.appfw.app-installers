/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#ifndef TPK_STEP_STEP_CREATE_SYMBOLIC_LINK_H_
#define TPK_STEP_STEP_CREATE_SYMBOLIC_LINK_H_

#include "common/app_installer.h"

namespace tpk {
namespace step {

class StepCreateSymbolicLink : public common_installer::Step {
 public:
  using Step::Step;
  Status process() override;
  Status clean() override;
  Status undo() override;
  Status precheck() override { return Status::OK; }
};

}  // namespace step
}  // namespace tpk

#endif  // TPK_STEP_STEP_CREATE_SYMBOLIC_LINK_H_
