/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#ifndef TPK_STEP_STEP_PARSE_H_
#define TPK_STEP_STEP_PARSE_H_

#include "boost/filesystem.hpp"
#include "common/step/step.h"
#include "tpk/manifest_parser.h"

namespace tpk {
namespace step {

class StepParse : public common_installer::Step {
 public:
  using Step::Step;

  Status process()  override;
  Status clean()    override { return Status::OK; };
  Status undo()     override { return Status::OK; };


 private:
  boost::filesystem::path
    GetManifestFilePath(const boost::filesystem::path& dir);
  void SetContextByManifestParser(const ManifestParser &m);
  void SetPkgInfoManifest(manifest_x* m, const XmlNodeManifest &manifest);
};

}  // namespace step
}  // namespace tpk

#endif  // TPK_STEP_STEP_PARSE_H_
