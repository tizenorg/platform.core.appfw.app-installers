// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_acquire_external_storage.h"

#include "common/external_storage.h"

namespace {

const char kInstalledExternally[] = "installed_external";
const char kPreferExternal[] = "prefer-external";

}  // namespace

namespace common_installer {
namespace filesystem {

Step::Status StepAcquireExternalStorage::precheck() {
  if (!context_->manifest_data.get()) {
    LOG(ERROR) << "Manifest data is empty";
    return Status::ERROR;
  }
  return Status::OK;
}

Step::Status StepAcquireExternalStorage::process() {
  manifest_x* manifest = context_->manifest_data.get();
  manifest_x* old_manifest = context_->old_manifest_data.get();

  bool installed_externally = false;
  if (old_manifest) {
    if (old_manifest->installed_storage)
      installed_externally =
          !strcmp(old_manifest->installed_storage, kInstalledExternally);
  } else {
    if (manifest->installed_storage)
      installed_externally =
          !strcmp(manifest->installed_storage, kInstalledExternally);
  }

  if (installed_externally ||
      !strcmp(manifest->installlocation, kPreferExternal)) {
    context_->external_storage =
        ExternalStorage::AcquireExternalStorage(context_->request_type.get(),
            context_->root_application_path.get(),
            context_->pkgid.get(),
            context_->unpacked_dir_path.get(),
            context_->uid.get());
  }

  if (installed_externally && !context_->external_storage) {
    LOG(ERROR) << "Cannot initialize external storage for installed package";
    return Status::APP_DIR_ERROR;
  }

  if (context_->external_storage) {
    // this may be allocated by step parse unfortunatelly to indicate storage
    // in case of recovery and update.
    free(const_cast<char*>(manifest->installed_storage));
    manifest->installed_storage = strdup(kInstalledExternally);
    LOG(INFO) << "Package storage: external";
  } else {
    LOG(INFO) << "Package storage: internal";
  }
  return Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer

