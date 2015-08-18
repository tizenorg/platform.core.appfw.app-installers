// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_STEP_WGT_COPY_2SD_H_
#define WGT_STEP_STEP_WGT_COPY_2SD_H_

#include <app2ext_interface.h>

#include <string>

#include "common/step/step_copy.h"
#include "common/wgt_external_storage.h"
#include "wgt/wgt_backend_data.h"

namespace wgt {
namespace filesystem {

class StepWgtCopy2SD : public common_installer::filesystem::StepCopy {
 public:
  using StepCopy::StepCopy;

  Status precheck() override;
  Status process() override;
  Status undo() override;
  Status clean() override;

 private:
  bool IsToInstallExternal(parse::SettingInfo::InstallLocation location);
  WgtBackendData* backend_data_;
  const char* pkgid_;
};

}  //  namespace filesystem
}  //  namespace wgt

#endif  //  WGT_STEP_STEP_WGT_COPY_2SD_H_

