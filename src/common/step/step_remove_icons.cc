// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_remove_icons.h"

#include <boost/system/error_code.hpp>
#include <pkgmgr-info.h>

#include <cstring>

#include "common/backup_paths.h"
#include "common/utils/clist_helpers.h"
#include "common/utils/file_util.h"

namespace common_installer {
namespace filesystem {

namespace bs = boost::system;
namespace fs = boost::filesystem;

Step::Status StepRemoveIcons::precheck() {
  if (!context_->manifest_data.get()) {
    LOG(ERROR) << "manifest_data attribute is empty";
    return Status::ERROR;
  }

  return Status::OK;
}

Step::Status StepRemoveIcons::process() {
  LOG(ERROR) << "hereeeeeeee";
  uiapplication_x* ui = nullptr;
  PKGMGR_LIST_MOVE_NODE_TO_HEAD(context_->manifest_data.get()->uiapplication,
                                ui);
  if (!context_->manifest_data.get())
    LOG(ERROR) << "is emptyyyy";
  for (; ui != nullptr; ui = ui->next) {
    LOG(ERROR) << "entered loop";
    fs::path app_icon = fs::path(getIconPath(context_->uid.get()))
      / fs::path(ui->appid);
    LOG(ERROR) << "app icon path " << app_icon.native();
    if (ui->icon && ui->icon->text)
      app_icon += fs::path(ui->icon->text).extension();
    else
      app_icon += ".png";
    LOG(ERROR) << "here app icon path new is " << app_icon.native();
    if (fs::exists(app_icon)) {
      LOG(ERROR) << "im here";
      fs::path backup_icon_file = GetBackupPathForIconFile(app_icon);
      LOG(ERROR) <<  "hereeee" << backup_icon_file;
      if (!MoveFile(app_icon, backup_icon_file)) {
        LOG(ERROR) << "Failed to create backup for icon: " << app_icon;
        return Status::ERROR;
      }
      backups_.emplace_back(backup_icon_file, app_icon);
    }
  }
  return Status::OK;
}

Step::Status StepRemoveIcons::clean() {
  bs::error_code error;
  if (!backups_.empty()) {
    LOG(DEBUG) << "Clean up icons files...";
    for (auto& pair : backups_) {
      fs::remove(pair.first, error);
      if (error) {
        LOG(WARNING) << "Failed to remove: " << pair.first;
      }
    }
  }
  return Status::OK;
}

Step::Status StepRemoveIcons::undo() {
  Step::Status ret = Status::OK;
  if (!backups_.empty()) {
    LOG(DEBUG) << "Restoring icons files...";
    for (auto& pair : backups_) {
      if (!MoveFile(pair.first, pair.second)) {
        LOG(ERROR) << "Failed to restore: " << pair.second;
        // We need to try to restore all icons anyway...
        ret = Status::ERROR;
      }
    }
  }
  return ret;
}
}  // namespace filesystem
}  // namespace common_installer
