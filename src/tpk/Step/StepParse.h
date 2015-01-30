/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#ifndef TPK_STEP_STEPPARSE_H_
#define TPK_STEP_STEPPARSE_H_

#include "common/step/step.h"
#include "tpk/ManifestParser.h"
#include "boost/filesystem.hpp"

namespace tpk {
namespace Step {

class StepParse : public common_installer::Step {
 public:
  using Step::Step;

  Status process()  override;
  Status clean()    override { return Status::OK; };
  Status undo()     override { return Status::OK; };


 private:
  boost::filesystem::path
    getManifestFilePath(const boost::filesystem::path& dir);
  void setContextByManifestParser(const ManifestParser &m);
  void setPkgInfoManifest(manifest_x* m, const XmlNodeManifest &manifest);
};

}  // namespace Step
}  // namespace tpk

#endif  // TPK_STEP_STEPPARSE_H_
