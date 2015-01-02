/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#ifndef COMMON_INCLUDE_STEP_STEP_UNZIP_H_
#define COMMON_INCLUDE_STEP_STEP_UNZIP_H_

#include <boost/filesystem/path.hpp>

#include "include/context_installer.h"
#include "include/step/step.h"

namespace common {
namespace unzip {

class StepUnzip : public Step {
 private:
  bool is_extracted_;
 public:
  StepUnzip();

  int process(ContextInstaller* context) override;
  int clean(ContextInstaller* /*context*/) override { return 0; }
  int undo(ContextInstaller* context) override;

  boost::filesystem::path GenerateTmpDir(const char*);
  int ExtractToTmpDir(const char* source_dir,
                      const boost::filesystem::path& tmp_dir);
};

}  // namespace unzip
}  // namespace common


#endif  // COMMON_INCLUDE_STEP_STEP_UNZIP_H_

