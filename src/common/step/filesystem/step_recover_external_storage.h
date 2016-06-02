// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_FILESYSTEM_STEP_RECOVER_EXTERNAL_STORAGE_H_
#define COMMON_STEP_FILESYSTEM_STEP_RECOVER_EXTERNAL_STORAGE_H_

#include "common/installer_context.h"
#include "common/step/filesystem/step_acquire_external_storage.h"

namespace common_installer {
namespace filesystem {

/**
 * \brief Creates external storage according to:
 *        1) package installation
 *        2) manifest file
 */
class StepRecoverExternalStorage : public StepAcquireExternalStorage {
 public:
  using StepAcquireExternalStorage::StepAcquireExternalStorage;

  Status process() override;

  STEP_NAME(RecoverExternalStorage)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_FILESYSTEM_STEP_RECOVER_EXTERNAL_STORAGE_H_
