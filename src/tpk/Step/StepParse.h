#ifndef __TPK_STEP_STEPPARSE_H__
#define __TPK_STEP_STEPPARSE_H__

#include "boost/filesystem.hpp"
#include "common/step/step.h"
#include "ManifestParser.h"

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
    void setContextByManifestParser(ManifestParser &m);
    void setPkgInfoManifest(manifest_x* m, XmlNodeManifest &manifest);

};

}  // end ns: Step
}  // end ns: tpk

#endif  // __TPK_STEP_STEPPARSE_H__
