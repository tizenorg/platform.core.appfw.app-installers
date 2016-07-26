// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_remove_globalapp_symlinks.h"

#include <string>

#include "common/installer_context.h"
#include "common/pkgdir_tool_request.h"
#include "common/pkgmgr_query.h"

namespace common_installer {
namespace filesystem {

Step::Status StepRemoveGlobalAppSymlinks::process() {
  if (context_->is_preload_request.get())
    return Step::Status::OK;

  if (!QueryIsPackageInstalled(context_->pkgid.get(), RequestMode::GLOBAL))
    return Step::Status::OK;

  std::string package_id = context_->pkgid.get();

  if (context_->request_type.get() == RequestType::Uninstall) {
    if (context_->request_mode.get() == RequestMode::GLOBAL) {
      LOG(INFO) << "Deleting globalapp symlinks for all user, package: "
                << package_id;
      RequestDeleteGlobalAppSymlinks(package_id);
    }
  } else {
    LOG(INFO) << "Deleting globalapp symlinks for current user, package: "
                << package_id;
    RequestDeleteGlobalAppSymlinksForUser(package_id, context_->uid.get());
  }

  return Step::Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer

