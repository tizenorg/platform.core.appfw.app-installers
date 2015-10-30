// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_backup_icons.h"

#include <pkgmgr-info.h>

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

#include "common/backup_paths.h"
#include "common/utils/file_util.h"
#include "common/utils/glist_range.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace common_installer {
namespace backup {

Step::Status StepBackupIcons::process() {
  // gather icon info
  for (application_x* app :
      GListRange<application_x*>(
         context_->old_manifest_data.get()->application)) {
    bf::path app_icon = bf::path(getIconPath(context_->uid.get()))
        / bf::path(app->appid);
    if (app->icon) {
      icon_x* icon = reinterpret_cast<icon_x*>(app->icon->data);
      if (!icon->text) {
        LOG(ERROR) << "Icon text is not set";
        return Status::ERROR;
      }
      app_icon += bf::path(icon->text).extension();
    } else {
      app_icon += ".png";
    }
    bf::path icon_backup = GetBackupPathForIconFile(app_icon);
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
      return Status::ERROR;
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
