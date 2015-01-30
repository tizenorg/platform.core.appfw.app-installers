/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#ifndef  COMMON_STEP_STEP_GENERATE_XML_H_
#define  COMMON_STEP_STEP_GENERATE_XML_H_


#include <boost/filesystem/path.hpp>

#include "common/context_installer.h"
#include "common/step/step.h"

namespace common_installer {
namespace generate_xml {

class StepGenerateXml : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override;
  Status undo() override;

 private:
  boost::filesystem::path icon_path_;
};

}  // namespace generate_xml
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_GENERATE_XML_H_
