// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_update_tep.h"

#include <pkgmgr-info.h>

#include <boost/filesystem.hpp>

#include <cstring>
#include <string>

#include "common/paths.h"
#include "common/pkgmgr_registration.h"
#include "common/utils/file_util.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace common_installer {
namespace filesystem {

Step::Status StepUpdateTep::precheck() {
  if (!context_->old_manifest_data.get()) {
    LOG(ERROR) << "Old manifest data is not set";
    return Status::INVALID_VALUE;
  }
  return StepCopyTep::precheck();
}

Step::Status StepUpdateTep::process() {
  // copy new tep file to package path if possible
  Status status = StepCopyTep::process();
  if (status != Status::OK)
    return status;

  // preserve old tep path if no new tep is supplied
  if (context_->tep_path.get().empty() &&
      context_->old_manifest_data.get()->tep_name) {
    context_->manifest_data.get()->tep_name =
        strdup(context_->old_manifest_data.get()->tep_name);

    // TODO(t.iwanek): some factoring in more inteligent way would be great for
    // this. Tep doesn't need to be copied in mount update as this mode is not
    // creating new package directory so tep is in place.
    if (context_->request_type.get() == RequestType::MountUpdate)
      return Status::OK;

    // copy if necessary
    if (!context_->external_storage) {
      bf::path new_path = context_->manifest_data.get()->tep_name;
      bf::path backup_path =
          GetInternalTepPath(
              GetBackupPathForPackagePath(context_->pkg_path.get()));
      backup_path /= new_path.filename();
      if (!bf::exists(new_path.parent_path())) {
        bs::error_code error;
        bf::create_directory(new_path.parent_path(), error);
        if (error) {
          LOG(ERROR) << "Cannot recreate directory for tep file";
          return Status::APP_DIR_ERROR;
        }
      }
      if (!CopyFile(backup_path, new_path)) {
        LOG(ERROR) << "Failed to preserve tep file during update";
        return Status::APP_DIR_ERROR;
      }
    }
  }
  return Status::OK;
}

Step::Status StepUpdateTep::clean() {
  if (context_->external_storage) {
    if (context_->old_manifest_data.get()->tep_name &&
        strcmp(context_->old_manifest_data.get()->tep_name,
               context_->manifest_data.get()->tep_name)) {
      bf::path old_tep = context_->old_manifest_data.get()->tep_name;
      bs::error_code error;
      bf::remove(old_tep, error);
      if (error) {
        LOG(WARNING) << "Failed to cleanup old tep package from sd card";
      }
    }
  }
  return Status::OK;
}

Step::Status StepUpdateTep::undo() {
  bs::error_code error;
  if (bf::exists(context_->tep_path.get())) {
    bf::remove_all(context_->tep_path.get(), error);
  }
  return Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer
