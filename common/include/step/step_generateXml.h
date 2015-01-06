/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#ifndef COMMON_INCLUDE_STEP_STEP_GENERATEXML_H_
#define COMMON_INCLUDE_STEP_STEP_GENERATEXML_H_

#include "include/context_installer.h"

#include <boost/filesystem.hpp>

#include "include/step/step.h"

namespace common_installer {
namespace generate_xml {

namespace fs = boost::filesystem;

class StepGenerateXml : public Step {
 private:
  fs::path icon_path_;
  fs::path xml_path_;
 public:
  int process(ContextInstaller* context) override;
  int clean(ContextInstaller* context) override;
  int undo(ContextInstaller* context) override;
};

}  // namespace generate_xml
}  // namespace common_installer

#endif  // COMMON_INCLUDE_STEP_STEP_GENERATEXML_H_
