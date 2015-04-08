/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#ifndef TPK_STEP_STEP_PARSE_H_
#define TPK_STEP_STEP_PARSE_H_

#include <boost/filesystem.hpp>
#include "common/step/step.h"
#include "xml_parser/xml_parser.h"

namespace tpk {
namespace step {

class StepParse : public common_installer::Step {
 public:
  using Step::Step;

  Status process()  override;
  Status clean()    override { return Status::OK; };
  Status undo()     override { return Status::OK; };
  Status precheck() override { return Status::OK; }


 private:
  boost::filesystem::path* GetManifestFilePath(
      const boost::filesystem::path& dir);
  bool SetContextByManifestParser(xml_parser::XmlTree* tree);
  bool SetPkgInfoManifest(manifest_x* m,
      xml_parser::XmlTree* tree, xml_parser::XmlElement* manifest);
  bool SetPkgInfoChildren(manifest_x *m,
      xml_parser::XmlTree *tree, xml_parser::XmlElement* manifest);
};

}  // namespace step
}  // namespace tpk

#endif  // TPK_STEP_STEP_PARSE_H_
