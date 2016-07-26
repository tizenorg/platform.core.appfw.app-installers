// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_create_globalapp_symlinks.h"

#include <string>

#include "common/pkgdir_tool_request.h"
#include "common/shared_dirs.h"
#include "common/pkgmgr_query.h"

namespace common_installer {
namespace filesystem {

common_installer::Step::Status StepCreateGlobalAppSymlinks::process() {
  if (context_->is_preload_request.get())
    return Step::Status::OK;
  std::string package_id = context_->pkgid.get();

  if (context_->request_type.get() == RequestType::Uninstall) {
    if ((context_->request_mode.get() == RequestMode::USER) &&
        QueryIsPackageInstalled(context_->pkgid.get(), RequestMode::GLOBAL)) {
      LOG(INFO) << "Creating globalapp symlinks for current user, package: "
              << package_id;
      RequestCreateGlobalAppSymlinksForUser(package_id, context_->uid.get());
    }
  } else {
    if (context_->request_mode.get() == RequestMode::GLOBAL) {
      LOG(INFO) << "Creating globalapp symlinks for all user, package: "
                << package_id;
      RequestCreateGlobalAppSymlinks(package_id);
    }
  }

  return Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer
