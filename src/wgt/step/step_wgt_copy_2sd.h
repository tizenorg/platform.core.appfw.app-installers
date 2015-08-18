// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_STEP_WGT_COPY_2SD_H_
#define WGT_STEP_STEP_WGT_COPY_2SD_H_

#include <string>

#include "common/step/step_copy.h"
#include "common/wgt_external_storage.h"
#include "wgt/wgt_backend_data.h"

namespace ci = common_installer;

namespace wgt {
namespace filesystem {

class StepWgtCopy2SD : public ci::filesystem::StepCopy {
 public:
  using StepCopy::StepCopy;

  Status precheck() override;
  Status process() override;
  Status undo() override;
  Status clean() override;

 private:
  bool IsToInstallExternal();
  parse::SettingInfo::InstallLocation location_;
  std::string pkgid_;
  WgtBackendData* backend_data_;
  ci::WgtExternalStorage* storage_;
};

}  //  namespace filesystem
}  //  namespace wgt

#endif  //  WGT_STEP_STEP_WGT_COPY_2SD_H_

