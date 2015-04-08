/* 2014, Copyright © Samsung, license APACHE-2.0, see LICENSE file */

#ifndef WGT_STEP_STEP_PARSE_H_
#define WGT_STEP_STEP_PARSE_H_

#include <boost/filesystem.hpp>

#include <memory>
#include <set>
#include <string>

#include "common/app_installer.h"
#include "common/context_installer.h"
#include "common/step/step.h"
#include "parser/manifest_parser.h"
#include "utils/logging.h"
#include "manifest_handlers/permissions_handler.h"

namespace wgt {
namespace parse {

class StepParse : public common_installer::Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override { return Status::OK; }

 private:
  std::set<std::string> ExtractPrivileges(
      std::shared_ptr<const PermissionsInfo> perm_info) const;
  bool FillManifestX(manifest_x* manifest);

  std::unique_ptr<parser::ManifestParser> parser_;
  boost::filesystem::path config_;
  bool Check(const boost::filesystem::path& widget_path);

  SCOPE_LOG_TAG(Parse)
};

}  // namespace parse
}  // namespace wgt

#endif  // WGT_STEP_STEP_PARSE_H_
