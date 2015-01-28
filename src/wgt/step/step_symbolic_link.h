/* 2014, Copyright © Samsung, license APACHE-2.0, see LICENSE file */

#ifndef WGT_STEP_STEP_SYMBOLIC_LINK_H_
#define WGT_STEP_STEP_SYMBOLIC_LINK_H_

#include <boost/filesystem.hpp>

#include "common/app_installer.h"
#include "common/context_installer.h"
#include "common/step/step.h"

namespace wgt {
namespace symbolic_link {

class StepSymbolicLink : public common_installer::Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override;
  Status undo() override;
};

}  // namespace symbolic_link
}  // namespace wgt

#endif  // WGT_STEP_STEP_SYMBOLIC_LINK_H_
