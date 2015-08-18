#ifndef STEP_WGT_COPY2SD_H
#define STEP_WGT_COPY2SD_H

#include <app2ext_interface.h>
#include "common/step/step.h"
#include "common/context_installer.h"
#include "common/step/step_copy.h"
#include "common/utils/logging.h"
#include "wgt/wgt_backend_data.h"

namespace wgt {
namespace filesystem {

class StepWgtCopy2SD : public common_installer::filesystem::StepCopy {
 public:
  using StepCopy::StepCopy;

  Status precheck() override;
  Status process() override;
  Status clean() override;
  Status undo() override;

 private:
  bool PreInstallation();
  void PostInstallation(bool status);
  WgtBackendData* backend_data_;
  app2ext_handle *app2_handle;
  std::string appname;
  int size;

};

}  // namespace filesystem
}  // namespace wgt

#endif // STEP_WGT_COPY2SD_H
