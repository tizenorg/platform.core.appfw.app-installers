/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#ifndef TPK_STEP_STEP_PARSE_H_
#define TPK_STEP_STEP_PARSE_H_

#include "common/step/step.h"
#include "tpk/xml_parser/xml_parser.h"

namespace tpk {
namespace parse {

class StepParse : public common_installer::Step {
 public:
  using Step::Step;

  Status process()  override;
  Status clean()    override { return Status::OK; };
  Status undo()     override { return Status::OK; };
  Status precheck() override;

 private:
  bool SetContextByManifestParser(xml_parser::XmlTree* tree);
  bool SetPkgInfoManifest(manifest_x* m,
      xml_parser::XmlTree* tree, xml_parser::XmlElement* manifest);
  bool SetPkgInfoChildren(manifest_x *m,
      xml_parser::XmlTree *tree, xml_parser::XmlElement* manifest);
};

}  // namespace parse
}  // namespace tpk

#endif  // TPK_STEP_STEP_PARSE_H_