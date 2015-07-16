// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_backup_icons.h"

#include <pkgmgr-info.h>

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

#include "common/utils/file_util.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace common_installer {
namespace backup {

Step::Status StepBackupIcons::process() {
  uiapplication_x* ui = context_->old_manifest_data.get()->uiapplication;

  // gather icon info
  for (; ui != nullptr; ui = ui->next) {
    bf::path app_icon = bf::path(getIconPath(context_->uid.get()))
      / bf::path(ui->appid);
    app_icon += ".png";
    bf::path icon_backup = app_icon;
    icon_backup += ".bck";
    if (bf::exists(app_icon))
        icons_.emplace_back(app_icon, icon_backup);
  }

  // backup
  for (auto& pair : icons_) {
    if (!MoveFile(pair.first, pair.second)) {
      LOG(ERROR) << "Cannot create backup for icon: " << pair.first;
      return Status::ERROR;
    }
  }

  LOG(DEBUG) << "Icons backup created";
  return Status::OK;
}

Step::Status StepBackupIcons::clean() {
  RemoveBackupIcons();
  LOG(DEBUG) << "Icons backup removed";
  return Status::OK;
}

Step::Status StepBackupIcons::undo() {
  for (auto& pair : icons_) {
    if (!MoveFile(pair.second, pair.first)) {
      LOG(ERROR) << "Cannot revert icon from backup: " << pair.first;
//      return Status::ERROR; // undo cannot fail, so no break
    }
  }
  LOG(DEBUG) << "Icons reverted from backup";
  return Status::OK;
}

void StepBackupIcons::RemoveBackupIcons() {
  for (auto& pair : icons_) {
    bs::error_code error;
    bf::remove(pair.second, error);
    if (error)
      LOG(WARNING) << "Cannot remove backup for icon: " << pair.second;
  }
}

}  // namespace backup
}  // namespace common_installer
