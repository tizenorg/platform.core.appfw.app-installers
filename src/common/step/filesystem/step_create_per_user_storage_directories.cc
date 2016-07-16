// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_create_per_user_storage_directories.h"

#include <string>
#include <vector>

#include "common/privileges.h"

#include "common/pkgdir_tool_request.h"
#include "common/shared_dirs.h"
#include "common/utils/glist_range.h"

namespace common_installer {
namespace filesystem {

common_installer::Step::Status StepCreatePerUserStorageDirectories::process() {
  std::string package_id = context_->pkgid.get();

  if (context_->request_mode.get() != RequestMode::GLOBAL) {
    common_installer::RequestCreateLegacyDirectories(package_id);
    return Step::Status::OK;
  }

  LOG(INFO) << "Creating per-user directories for package: " << package_id;

  manifest_x* manifest = context_->manifest_data.get();
  std::string str_ver(manifest->api_version);
  std::string author_id = context_->certificate_info.get().author_id.get();
  bool trusted = false;
  if (!author_id.empty())
    trusted = true;

  if (!common_installer::CreateSkelDirectories(package_id,
      str_ver, trusted)) {
    LOG(ERROR) << "Failed to create skel dirs";
    return Status::APP_DIR_ERROR;
  }

  if (!common_installer::RequestCopyUserDirectories(package_id)) {
    LOG(ERROR) << "Failed to create shared dirs for users";
    return Status::APP_DIR_ERROR;
  }

  common_installer::RequestCreateLegacyDirectories(package_id);

  return Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer
