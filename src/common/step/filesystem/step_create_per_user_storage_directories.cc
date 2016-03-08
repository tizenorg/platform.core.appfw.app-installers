// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include <string>
#include <vector>

#include "common/utils/subprocess.h"
#include "common/step/filesystem/step_create_per_user_storage_directories.h"

#include "common/shared_dirs.h"
#include "common/utils/glist_range.h"

namespace common_installer {
namespace filesystem {

common_installer::Step::Status StepCreatePerUserStorageDirectories::process() {
  if (!CreateExternalStorageDir()) return Step::Status::APP_DIR_ERROR;
  if (GLOBAL_USER != context_->uid.get()) return Step::Status::OK;

  std::string package_id = context_->pkgid.get();
  LOG(INFO) << "Creating per-user directories for package: " << package_id;

  Subprocess pkgdir_tool_process("/usr/bin/pkgdir-tool");
  pkgdir_tool_process.RunWithArgs({"--create", "--pkgid", package_id});
  int result = pkgdir_tool_process.Wait();
  if (result) {
    LOG(ERROR) << "Failed to create shared dirs for users";
    return Status::APP_DIR_ERROR;
  }
  return Status::OK;
}

bool StepCreatePerUserStorageDirectories::CreateExternalStorageDir() {
  auto manifest = context_->manifest_data.get();
    bool has_external_storage_priv = false;
    const char* privilege =
        "http://tizen.org/privilege/externalstorage.appdata";
    for (const char* priv : GListRange<char*>(manifest->privileges)) {
      if (strcmp(priv, privilege) == 0) {
        has_external_storage_priv = true;
        LOG(DEBUG) << "External storage privilege has been found.";
        break;
      }
    }
    if (!has_external_storage_priv) {
      LOG(DEBUG) << "External storage privilege not found, skipping.";
      return true;
    }
    std::vector<std::string> pkg_ids = { context_->pkgid.get() };
    auto pkg_list = CreatePkgInformationList(context_->uid.get(), pkg_ids);

    if (pkg_list.empty()) {
      LOG(ERROR) << "Could not create pkg information list.";
      return false;
    }

    PkgInfo pkg = *pkg_list.begin();
    bool create_skel_dirs = false;

    switch (context_->request_mode.get()) {
      case RequestMode::GLOBAL: {
        LOG(DEBUG) << "Creating external directories for all users";
        if (!PerformExternalDirectoryCreationForAllUsers(pkg.pkg_id,
                                                         pkg.author_id,
                                                         pkg.api_version,
                                                         create_skel_dirs))
          return false;
      }
      break;
      case RequestMode::USER: {
        LOG(DEBUG) << "Creating external directories for user: "
            << context_->uid.get();
        if (!PerformExternalDirectoryCreationForUser(context_->uid.get(),
                                                     pkg.pkg_id,
                                                     pkg.author_id,
                                                     pkg.api_version,
                                                     create_skel_dirs))
          return false;
      }
      break;
    }

    return true;
}

}  // namespace filesystem
}  // namespace common_installer
