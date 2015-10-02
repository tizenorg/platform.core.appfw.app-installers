// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/configuration/step_block_cross_update.h"

#include <boost/filesystem/operations.hpp>

#include "common/backup_paths.h"

namespace bf = boost::filesystem;

namespace common_installer {
namespace configuration {

Step::Status StepBlockCrossUpdate::process() {
  // TODO(t.iwanek): check for file is not the best method
  // information about if package was mount-installed should be stored in pkgmgr
  // database
  bool is_mount_installed =
      bf::exists(GetZipPackageLocation(
          context_->root_application_path.get() / context_->pkgid.get(),
          context_->pkgid.get()));
  if (is_mount_installed) {
    if (context_->request_type.get() == RequestType::Reinstall) {
      LOG(ERROR) << "Reinstall / RDS mode is not allowed for "
                 << "mount-installed packages";
      return Status::OPERATION_NOT_ALLOWED;
    }
    if (context_->request_type.get() == RequestType::Delta) {
      LOG(ERROR) << "Delta mode is not allowed for mount-installed packages";
      return Status::OPERATION_NOT_ALLOWED;
    }
    if (context_->request_type.get() == RequestType::Update) {
      LOG(ERROR) << "Normal update mode is not allowed for "
                 << "mount-installed packages";
      return Status::OPERATION_NOT_ALLOWED;
    }
  } else {
    if (context_->request_type.get() == RequestType::MountUpdate) {
      LOG(ERROR) << "Mount update mode is not allowed for "
                 << "installed packages in normal manner";
      return Status::OPERATION_NOT_ALLOWED;
    }
  }

  LOG(DEBUG) << "Update is allowed to proceed";
  return Status::OK;
}

}  // namespace configuration
}  // namespace common_installer
