#ifndef __TPK_STEP_STEPSYMBOLICLINK_H__
#define __TPK_STEP_STEPSYMBOLICLINK_H__

#include "common/app_installer.h"

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

};

} // namespace Step
} // namespace tpk

#endif  //__TPK_STEP_STEPSYMBOLICLINK_H__
