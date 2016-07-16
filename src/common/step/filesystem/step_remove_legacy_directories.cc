// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_remove_legacy_directories.h"

#include <string>

#include "common/installer_context.h"
#include "common/pkgdir_tool_request.h"

namespace common_installer {
namespace filesystem {

Step::Status StepRemoveLegacyDirectories::process() {
  std::string package_id = context_->pkgid.get();

  common_installer::RequestDeleteLegacyDirectories(package_id);

  return Step::Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer

