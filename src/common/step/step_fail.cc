// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_fail.h"

namespace common_installer {
namespace configuration {

Step::Status StepFail::process() {
  LOG(ERROR) << "Request was expected to fail";
  return Status::ERROR;
}

}  // namespace configuration
}  // namespace common_installer
