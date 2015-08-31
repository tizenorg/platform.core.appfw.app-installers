// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_STEP_WGT_COPY_STORAGE_DIRECTORIES_H_
#define WGT_STEP_STEP_WGT_COPY_STORAGE_DIRECTORIES_H_

#include "common/step/step_copy_storage_directories.h"
#include "utils/logging.h"

namespace wgt {
namespace filesystem {

/**
 * \brief Update installation (WGT).
 *        Responsible for coping shared and data directories.
 *        It extends StepCopyStorageDirectories (it adds distinction between
 *        2.x and 3.x shared dir handling
 */
class StepWgtCopyStorageDirectories
    : public common_installer::filesystem::StepCopyStorageDirectories {
 public:
  using StepCopyStorageDirectories::StepCopyStorageDirectories;

  Status process() override;
  Status undo() override;

 private:
  Status CopySharedDirectory();
  Status CopyDataDirectory();
  Status HandleWgtSharedDirectory();
  void UndoSharedDirectory();
  void UndoDataDirectory();
  Status CreatePrivateTmpDir();
  Status CreateCacheDir();

  SCOPE_LOG_TAG(CopyWgtStorageDirectories)
};

}  // namespace filesystem
}  // namespace wgt

#endif  // WGT_STEP_STEP_WGT_COPY_STORAGE_DIRECTORIES_H_
