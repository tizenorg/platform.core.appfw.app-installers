// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/recovery/step_recovery.h"

#include <manifest_parser/utils/logging.h>

#include "common/request.h"

namespace common_installer {
namespace recovery {

Step::Status StepRecovery::process() {
  switch (context_->recovery_info.get().recovery_file->type()) {
  case RequestType::Install:
    return RecoveryNew();
  case RequestType::Update:
    return RecoveryUpdate();
  default:
    LOG(ERROR) << "Recovery is not supported for given type of installation";
    return Status::RECOVERY_ERROR;
  }
}

}  // namespace recovery
}  // namespace common_installer
