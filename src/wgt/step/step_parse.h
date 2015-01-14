/* 2014, Copyright © Samsung, license APACHE-2.0, see LICENSE file */

#ifndef WGT_STEP_STEP_PARSE_H_
#define WGT_STEP_STEP_PARSE_H_

#include <boost/filesystem.hpp>

#include "common/app_installer.h"
#include "common/context_installer.h"
#include "common/step/step.h"

namespace wgt {
namespace parse {

//TODO MAYBE fill later
//class ConfigFileParser {
// public:
//  ConfigFileParser(char * file);
//};

class StepParse: public common_installer::Step {
 public:
  StepParse() {}

  int process(common_installer::ContextInstaller* context) override;
  int clean(common_installer::ContextInstaller*) override { return 0; }
  int undo(common_installer::ContextInstaller*) override { return 0; }

 private:
  boost::filesystem::path config_;
  bool Check(const boost::filesystem::path& widget_path);
  void fillManifest();
};

}  // namespace parse
}  // namespace wgt

#endif  // WGT_STEP_STEP_PARSE_H_
