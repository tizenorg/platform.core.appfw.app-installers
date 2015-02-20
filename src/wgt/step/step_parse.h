/* 2014, Copyright © Samsung, license APACHE-2.0, see LICENSE file */

#ifndef WGT_STEP_STEP_PARSE_H_
#define WGT_STEP_STEP_PARSE_H_

#include <boost/filesystem.hpp>

#include "common/app_installer.h"
#include "common/context_installer.h"
#include "common/step/step.h"
#include "parser/widget_manifest_parser.h"
#include "utils/logging.h"

namespace wgt {
namespace parse {

class StepParse : public common_installer::Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }

 private:
  std::unique_ptr<
      common_installer::parser::WidgetManifestParser>
          parser_;
  boost::filesystem::path config_;
  bool Check(const boost::filesystem::path& widget_path);

  SCOPE_LOG_TAG(Parse)
};

}  // namespace parse
}  // namespace wgt

#endif  // WGT_STEP_STEP_PARSE_H_
