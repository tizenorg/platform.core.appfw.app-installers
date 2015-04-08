/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#ifndef COMMON_STEP_STEP_UNZIP_H_
#define COMMON_STEP_STEP_UNZIP_H_

#include <boost/filesystem/path.hpp>

#include "common/context_installer.h"
#include "common/step/step.h"
#include "utils/logging.h"

namespace common_installer {
namespace unzip {

class StepUnzip : public Step {
 public:
  explicit StepUnzip(ContextInstaller* context);

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override;
  Status precheck() override;

 private:
  boost::filesystem::path GenerateTmpDir(
      const boost::filesystem::path& app_path);
  Step::Status ExtractToTmpDir(const char* source_dir,
      const boost::filesystem::path& tmp_dir);

  bool is_extracted_;

  SCOPE_LOG_TAG(Unzip)
};

}  // namespace unzip
}  // namespace common_installer


#endif  // COMMON_STEP_STEP_UNZIP_H_
