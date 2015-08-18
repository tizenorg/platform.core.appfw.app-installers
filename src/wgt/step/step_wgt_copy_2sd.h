// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_STEP_WGT_COPY_2SD_H_
#define WGT_STEP_STEP_WGT_COPY_2SD_H_

#include <app2ext_interface.h>

#include <string>

#include "common/context_installer.h"
#include "common/step/step.h"
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
  bool External(parse::SettingInfo::InstallLocation location);
  bool PreInstallation(std::string appname);
  void PostInstallation(bool status, std::string appname);
  WgtBackendData* backend_data_ = NULL;
  app2ext_handle *app2_handle_ = NULL;
};

}  //  namespace filesystem
}  //  namespace wgt

#endif  //  WGT_STEP_STEP_WGT_COPY_2SD_H_

