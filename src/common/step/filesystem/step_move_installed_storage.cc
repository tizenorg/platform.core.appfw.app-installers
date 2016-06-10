// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_move_installed_storage.h"

#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>
#include <tzplatform_config.h>

#include <string>

#include "common/backup_paths.h"
#include "common/external_storage.h"
#include "common/utils/file_util.h"
#include "common/pkgmgr_registration.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace common_installer {
namespace filesystem {

Step::Status StepMoveInstalledStorage::process() {
  context_->external_storage =
      ExternalStorage::MoveInstalledStorage(context_->request_type.get(),
          context_->root_application_path.get(),
          context_->pkgid.get(),
          context_->pkg_type.get(),
          context_->uid.get(),
          context_->is_move_to_external.get());
  if (!context_->external_storage) {
    LOG(ERROR) << "Cannot initialize external storage for move";
    return Status::APP_DIR_ERROR;
  }

  // move tep
  if (context_->manifest_data.get()->tep_name) {
    SetTepPaths();

    if (new_location_ != old_location_)
      if (!MoveTep())
        return Status::APP_DIR_ERROR;
  }
  return Status::OK;
}

Step::Status StepMoveInstalledStorage::undo() {
  if (context_->external_storage) {
    if (new_location_ != old_location_)
      if (!MoveBackTep())
        return Status::APP_DIR_ERROR;
    context_->external_storage->Abort();
  }
  return Status::OK;
}

void StepMoveInstalledStorage::SetTepPaths() {
  old_location_ = context_->manifest_data.get()->tep_name;
  if (context_->is_move_to_external.get()) {
    new_location_ = GetExternalTepPath(context_->request_mode.get(),
                                       context_->uid.get());
    new_location_ /= old_location_.filename();
  } else {
    new_location_ =
        GetInternalTepPath(context_->pkg_path.get()) / old_location_.filename();
  }
}

bool StepMoveInstalledStorage::MoveTep() {
  if (!bf::exists(new_location_.parent_path())) {
    bs::error_code error;
    bf::create_directory(new_location_.parent_path(), error);
    if (error) {
      LOG(ERROR) << "Failed to destination path for new tep location";
      return false;
    }
  }

  if (!MoveFile(old_location_, new_location_)) {
    LOG(ERROR) << "Cannot move tep file from: " << old_location_
               << " to " << new_location_;
    return false;
  }

  if (!UpdateTepInfoInPkgmgr(new_location_, context_->pkgid.get(),
      context_->uid.get(), context_->request_mode.get())) {
    LOG(ERROR) << "Failed to update tep package location in pkgmgr";
    return false;
  }
  return true;
}

bool StepMoveInstalledStorage::MoveBackTep() {
  if (bf::exists(new_location_)) {
    if (!MoveFile(new_location_, old_location_)) {
      LOG(ERROR) << "Cannot move tep file from: " << new_location_
                 << " to " << old_location_;
      return false;
    }

    if (!UpdateTepInfoInPkgmgr(old_location_, context_->pkgid.get(),
        context_->uid.get(), context_->request_mode.get())) {
      LOG(ERROR) << "Failed to update tep package location in pkgmgr";
      return false;
    }
  }
  return true;
}

}  // namespace filesystem
}  // namespace common_installer

