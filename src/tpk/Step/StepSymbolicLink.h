/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#ifndef TPK_STEP_STEPSYMBOLICLINK_H_
#define TPK_STEP_STEPSYMBOLICLINK_H_

#include "common/app_installer.h"
#include "utils/logging.h"


namespace tpk {
namespace Step {

class StepSymbolicLink : public common_installer::Step {
 public:
  using Step::Step;
  Status process() override;
  Status clean() override;
  Status undo() override;


 private:
  template <typename T> int createSymLink(T _app);
  template <typename T> int removeSymLink(T _app);

  SCOPE_LOG_TAG(SymbolicLink)
};

}  // namespace Step
}  // namespace tpk

#endif  // TPK_STEP_STEPSYMBOLICLINK_H_
