// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_create_legacy_directories.h"

#include <string>

#include "common/pkgdir_tool_request.h"
#include "common/shared_dirs.h"

namespace common_installer {
namespace filesystem {

common_installer::Step::Status StepCreateLegacyDirectories::process() {
  std::string package_id = context_->pkgid.get();

  LOG(INFO) << "Creating legacy directories for package: " << package_id;
  common_installer::RequestCreateLegacyDirectories(package_id);

  return Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer
