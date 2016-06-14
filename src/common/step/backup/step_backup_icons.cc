// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/backup/step_backup_icons.h"

#include <pkgmgr-info.h>

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

#include <string>

#include "common/backup_paths.h"
#include "common/utils/file_util.h"
#include "common/utils/glist_range.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace common_installer {
namespace backup {

Step::Status StepBackupIcons::process() {
  // gather icon info
  const char *extra_icon_path = getIconPath(context_->uid.get(),
      context_->is_preload_request.get());
  if (!extra_icon_path)
    return Status::OK;

  for (auto iter = bf::directory_iterator(extra_icon_path);
      iter != bf::directory_iterator(); ++iter) {
    if (!bf::is_regular_file(iter->path()))
      continue;
    for (application_x* app : GListRange<application_x*>(
        context_->old_manifest_data.get()->application)) {
      if (app->icon) {
        bf::path filename = iter->path().filename();
        filename.replace_extension();
        std::string id = filename.string();
        if (id == app->appid) {
          bf::path icon_backup = GetBackupPathForIconFile(iter->path());
          icons_.emplace_back(iter->path(), icon_backup);
        }
      }
    }
  }

  // backup
  for (auto& pair : icons_) {
    if (!MoveFile(pair.first, pair.second)) {
      LOG(ERROR) << "Cannot create backup for icon: " << pair.first;
      return Status::ICON_ERROR;
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
      return Status::ICON_ERROR;
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
