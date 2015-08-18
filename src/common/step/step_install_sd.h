// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_INSTALL_SD_H_
#define COMMON_STEP_STEP_INSTALL_SD_H_

#include "common/external_storage.h"
#include "common/step/step.h"
#include "common/utils/logging.h"
#include "wgt/wgt_backend_data.h"

namespace common_installer {
namespace filesystem {

class StepInstallSD : public Step {
 public:
  using Step::Step;

  Status precheck() override;
  Status process() override;
  Status undo() override;
  Status clean() override { return Step::Status::OK; }

 private:
  std::unique_ptr<ExternalStorage> storage_;

  SCOPE_LOG_TAG(InstallSD)
};
}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_INSTALL_SD_H_
