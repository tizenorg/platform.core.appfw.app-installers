// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_move_installed_storage.h"

#include "common/external_storage.h"

namespace {


}  // namespace

namespace common_installer {
namespace filesystem {

Step::Status StepMoveInstalledStorage::precheck() {
#if 1
  // if don't need to check anything, remove this
  return Status::OK;
#else
  if (!context_->manifest_data.get()) {
    LOG(ERROR) << "Manifest data is empty";
    return Status::ERROR;
  }
  return Status::OK;
#endif
}

Step::Status StepMoveInstalledStorage::process() {
#if 1

  //should we store returned object to context_->external_storage?
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

#else
  manifest_x* manifest = context_->manifest_data.get();
  manifest_x* old_manifest = context_->old_manifest_data.get();

  Storage storage = Storage::NONE;
  if (old_manifest) {
    storage = Storage::INTERNAL;
    if (old_manifest->installed_storage)
      if (!strcmp(old_manifest->installed_storage, kInstalledExternally))
        storage = Storage::EXTERNAL;
  }

  if (storage == Storage::EXTERNAL ||
      (!strcmp(manifest->installlocation, kPreferExternal) &&
      storage == Storage::NONE) ||
      context_->request_type.get() == RequestType::Move) {
    context_->external_storage =
        ExternalStorage::AcquireExternalStorage(context_->request_type.get(),
            context_->root_application_path.get(),
            context_->pkgid.get(),
            context_->pkg_type.get(),
            context_->unpacked_dir_path.get(),
            context_->uid.get(),
            context_->is_move_to_external.get());
  }

  if (storage == Storage::EXTERNAL && !context_->external_storage) {
    LOG(ERROR) << "Cannot initialize external storage for installed package";
    return Status::APP_DIR_ERROR;
  }

  if (context_->external_storage) {
    // This may be allocated by step parse unfortunatelly to indicate storage
    // in case of recovery and update.
    free(const_cast<char*>(manifest->installed_storage));
    manifest->installed_storage = strdup(kInstalledExternally);
    LOG(INFO) << "Package storage: external";
  } else {
    LOG(INFO) << "Package storage: internal";
  }
#endif
  return Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer

