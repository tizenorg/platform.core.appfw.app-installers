// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_COPY_2SD_H_
#define COMMON_STEP_STEP_COPY_2SD_H_

#include <memory>
#include <string>

#include "common/step/step_copy.h"
#include "common/external_storage.h"
#include "wgt/wgt_backend_data.h"


namespace common_installer {
namespace filesystem {

class StepCopy2SD : public StepCopy {
 public:
  using StepCopy::StepCopy;

  Status precheck() override;
  Status process() override;
  Status undo() override;
  Status clean() override;

 private:
  bool IsToInstallExternal();

  bool installed_external;
  const manifest_x* manifest_;
  std::string pkgid_;
  std::unique_ptr<ExternalStorage> storage_;
};

}  //  namespace filesystem
}  //  namespace common_installer

#endif  //  COMMON_STEP_STEP_COPY_2SD_H_

