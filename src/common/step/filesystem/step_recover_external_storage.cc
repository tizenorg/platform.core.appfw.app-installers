// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_recover_external_storage.h"

namespace common_installer {
namespace filesystem {

Step::Status StepRecoverExternalStorage::process() {
  (void) StepAcquireExternalStorage::process();
  return Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer

