// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_recover_package_storage.h"

namespace common_installer {
namespace filesystem {

Step::Status StepRecoverPackageStorage::process() {
  (void) StepAcquirePackageStorage::process();
  return Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer

