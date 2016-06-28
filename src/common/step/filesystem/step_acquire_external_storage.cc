// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_acquire_external_storage.h"

#include <vconf.h>
#include <vconf-internal-keys.h>

#include <cstring>

#include "common/external_storage.h"

namespace {

const char kInstalledExternally[] = "installed_external";
const char kInternalOnly[] = "internal-only";
const char kDefaultStorageVconfKey[] =
    VCONFKEY_SETAPPL_DEFAULT_MEM_INSTALL_APPLICATIONS_INT;

enum class Storage {
  NONE,
  INTERNAL,
  EXTERNAL
};

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

  Storage storage = Storage::NONE;
  // in case of update
  if (old_manifest) {
    storage = Storage::INTERNAL;
    if (old_manifest->installed_storage)
      if (!strcmp(old_manifest->installed_storage, kInstalledExternally))
        storage = Storage::EXTERNAL;
  } else {
    if (!strcmp(manifest->installlocation, kInternalOnly)) {
      storage = Storage::INTERNAL;
    } else {
      // int type vconf value:
      // 0 means internal storage, 1 means external storage
      int default_storage = 0;
      if (vconf_get_int(kDefaultStorageVconfKey, &default_storage))
        LOG(WARNING) << "Cannot get default storage type, set internal storage "
                     << "as default value";
      if (default_storage == 0)
        storage = Storage::INTERNAL;
      else
        storage = Storage::EXTERNAL;
    }
  }

  if (storage == Storage::EXTERNAL)
    context_->external_storage =
        ExternalStorage::AcquireExternalStorage(context_->request_type.get(),
            context_->root_application_path.get(),
            context_->pkgid.get(),
            context_->pkg_type.get(),
            context_->unpacked_dir_path.get(),
            context_->uid.get());

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
  return Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer

