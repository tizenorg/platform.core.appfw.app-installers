// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_STEP_WGT_CREATE_STORAGE_DIRECTORIES_H_
#define WGT_STEP_STEP_WGT_CREATE_STORAGE_DIRECTORIES_H_

#include "common/step/step_create_storage_directories.h"
#include "utils/logging.h"

namespace wgt {
namespace filesystem {

/**
 * \brief Installation (WGT).
 *        Responsible for creating shared and data directories.
 *        It extends StepCreateStorageDirectories (it adds distinction between
 *        2.x and 3.x shared dir handling
 *
 * process method implements creation of shared and data directories.
 * Depending on tizen required version it can also move "shared" resources
 * from ./res/wgt/shared to ./shared dir (and create symlink back
 * to ./res/wgt/shared).
 *
 * StepWgtCreateStorageDirectories works on following directory:
 * * TZ_SYS_RW/PKGID (/usr/apps/PKGID)
 * * TZ_SER_APPS/PKGID  (/{HOME}/apps_rw/PKGID)
 */
class StepWgtCreateStorageDirectories :
    public common_installer::filesystem::StepCreateStorageDirectories {
 public:
  using StepCreateStorageDirectories::StepCreateStorageDirectories;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override { return Status::OK; }

 private:
  bool ShareDirFor2x();
  bool ShareDirFor3x();

  bool CreatePrivateTmpDir();
  bool CreateCacheDir();

  SCOPE_LOG_TAG(CreateWgtStorageDirectories)
};

}  // namespace filesystem
}  // namespace wgt

#endif  // WGT_STEP_STEP_WGT_CREATE_STORAGE_DIRECTORIES_H_
