/* 2014, Copyright © Samsung, license APACHE-2.0, see LICENSE file */

#ifndef WGT_STEP_STEP_PARSE_H_
#define WGT_STEP_STEP_PARSE_H_

#include <boost/filesystem.hpp>

#include "common/app_installer.h"
#include "common/context_installer.h"
#include "common/step/step.h"

namespace wgt {
namespace parse {

class StepParse : public common_installer::Step {
 public:
  using Step::Step;

  Step::Status process() override;
  Step::Status clean() override { return Step::Status::OK; }
  Step::Status undo() override { return Step::Status::OK; }

 private:
  boost::filesystem::path config_;
  bool Check(const boost::filesystem::path& widget_path);
};

}  // namespace parse
}  // namespace wgt

#endif  // WGT_STEP_STEP_PARSE_H_
