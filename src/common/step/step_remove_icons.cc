// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_remove_icons.h"

#include <boost/system/error_code.hpp>
#include <pkgmgr-info.h>

#include <cstring>
#include <string>

#include "common/backup_paths.h"
#include "common/utils/file_util.h"
#include "common/utils/glist_range.h"

namespace common_installer {
namespace filesystem {

namespace bs = boost::system;
namespace bf = boost::filesystem;

Step::Status StepRemoveIcons::precheck() {
  if (!context_->manifest_data.get()) {
    LOG(ERROR) << "manifest_data attribute is empty";
    return Status::MANIFEST_NOT_FOUND;
  }

  return Status::OK;
}

Step::Status StepRemoveIcons::process() {
  for (auto iter = bf::directory_iterator(getIconPath(context_->uid.get()));
      iter != bf::directory_iterator(); ++iter) {
    if (!bf::is_regular_file(iter->path()))
      continue;
    for (application_x* app :
        GListRange<application_x*>(
           context_->manifest_data.get()->application)) {
      if (app->icon) {
        std::string filename = iter->path().filename().string();
        if (filename.find(app->appid) == 0) {
          bf::path icon_backup = GetBackupPathForIconFile(iter->path());
          backups_.emplace_back(iter->path(), icon_backup);
        }
      }
    }
  }
  return Status::OK;
}

Step::Status StepRemoveIcons::clean() {
  bs::error_code error;
  if (!backups_.empty()) {
    LOG(DEBUG) << "Clean up icons files...";
    for (auto& pair : backups_) {
      bf::remove(pair.first, error);
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
        ret = Status::ICON_ERROR;
      }
    }
  }
  return ret;
}
}  // namespace filesystem
}  // namespace common_installer
