// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_acquire_package_storage.h"

#include "common/package_storage.h"

namespace common_installer {
namespace filesystem {

Step::Status StepAcquirePackageStorage::precheck() {
  if (!context_->manifest_data.get()) {
    LOG(ERROR) << "Manifest data is empty";
    return Status::ERROR;
  }
  return Status::OK;
}

Step::Status StepAcquirePackageStorage::process() {
  manifest_x* manifest = context_->manifest_data.get();
  manifest_x* old_manifest = context_->old_manifest_data.get();
  context_->package_storage =
      AcquirePackageStorage(context_->request_type.get(),
          context_->root_application_path.get(),
          context_->pkgid.get(),
          context_->unpacked_dir_path.get(),
          old_manifest ?
              old_manifest->installed_storage : manifest->installed_storage,
          manifest->installlocation);
  if (!context_->package_storage) {
    LOG(ERROR) << "Failed to initialize package storage";
    return Status::APP_DIR_ERROR;
  }

  // this may be allocated by step parse unfortunatelly to indicate storage
  // in case of recovery and update.
  free(const_cast<char*>(manifest->installed_storage));

  manifest->installed_storage =
      strdup(context_->package_storage->StorageName().c_str());
  LOG(INFO) << "Using storage: " << context_->package_storage->StorageName();
  return Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer

