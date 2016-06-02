// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_move_installed_storage.h"

#include "common/external_storage.h"

namespace common_installer {
namespace filesystem {

Step::Status StepMoveInstalledStorage::process() {
  context_->external_storage =
      ExternalStorage::MoveInstalledStorage(context_->request_type.get(),
          context_->root_application_path.get(),
          context_->pkgid.get(),
          context_->pkg_type.get(),
          context_->uid.get(),
          context_->is_move_to_external.get());
  if (!context_->external_storage) {
    LOG(ERROR) << "Cannot initialize external storage for move";
    return Status::APP_DIR_ERROR;
  }

  return Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer

