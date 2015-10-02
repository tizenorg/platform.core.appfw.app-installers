// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/pkgmgr/step_update_tep.h"

#include <pkgmgr-info.h>

#include <boost/filesystem.hpp>

#include <cstring>
#include <string>

#include "common/backup_paths.h"
#include "common/pkgmgr_registration.h"
#include "common/utils/file_util.h"

namespace bf = boost::filesystem;

namespace common_installer {
namespace pkgmgr {

Step::Status StepUpdateTep::process() {
  bf::path old_tep;
  if (context_->old_manifest_data.get()->tep_name)
    old_tep = context_->old_manifest_data.get()->tep_name;
  if (!old_tep.empty() && context_->tep_path.get().empty()) {
    // preserve old tep location during update if no new is given
    context_->manifest_data.get()->tep_name = strdup(old_tep.c_str());

    bf::path old_tep_location =
        GetBackupPathForPackagePath(
            context_->package_storage->path()) / "res" / old_tep.filename();
    bf::path new_tep_location = old_tep;
    if (!MoveFile(old_tep_location, new_tep_location)) {
      LOG(ERROR) << "Failed to copy tep file";
      return Status::APP_DIR_ERROR;
    }
  }
  return Status::OK;
}

Step::Status StepUpdateTep::undo() {
  bf::path old_tep;
  if (context_->old_manifest_data.get()->tep_name)
    old_tep = context_->old_manifest_data.get()->tep_name;
  if (!old_tep.empty() && context_->tep_path.get().empty()) {
    // restore old tep location during update rollback
    bf::path old_tep_location =
        GetBackupPathForPackagePath(
            context_->package_storage->path()) / "res" / old_tep.filename();
    bf::path new_tep_location = old_tep;
    if (!MoveFile(new_tep_location, old_tep_location)) {
      LOG(ERROR) << "Failed to copy tep file";
      return Status::APP_DIR_ERROR;
    }
  }
  return Status::OK;
}

}  // namespace pkgmgr
}  // namespace common_installer
