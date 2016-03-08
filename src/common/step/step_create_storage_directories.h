// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_CREATE_STORAGE_DIRECTORIES_H_
#define COMMON_STEP_STEP_CREATE_STORAGE_DIRECTORIES_H_

#include <manifest_parser/utils/logging.h>

#include "common/step/step.h"

namespace common_installer {
namespace filesystem {

/**
 * \brief Installation.
 *        Responsible for creating shared and data directories (wgt/tpk)
 *
 * * process method implements creation of data and shared directories for
 *   package.
 * * Other methods are empty.
 *
 * CreateStorageDirectories works on below directories:
 * * context_->pkg_path.get(), eg:
 *   * TZ_SYS_RW/PKGID/<new-dir> (/usr/apps/PKGID/<new-dir>)
 *   * TZ_SER_APPS/PKGID/<new-dir>  (/{HOME}/apps_rw/PKGID/<new-dir>)
 */
class StepCreateStorageDirectories : public common_installer::Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override { return Status::OK; }

 protected:
  bool ShareDir();
  bool SubShareDir();
  bool PrivateDir();
  bool CacheDir();
  bool ExtStorageDir();

  SCOPE_LOG_TAG(CreateStorageDirectories)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_CREATE_STORAGE_DIRECTORIES_H_
