// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_recover_icons.h"

#include <boost/filesystem/operations.hpp>
#include <boost/system/error_code.hpp>
#include <pkgmgr-info.h>

#include "common/backup_paths.h"
#include "common/utils/clist_helpers.h"
#include "common/utils/file_util.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace common_installer {
namespace filesystem {

Step::Status StepRecoverIcons::RecoveryNew() {
  if (!TryGatherIcons()) {
    LOG(DEBUG) << "Icons recovery not needed";
    return Status::OK;
  }
  for (auto& pair : icons_) {
    bs::error_code error;
    bf::remove(pair.first, error);
  }
  LOG(INFO) << "Icons recovery done";
  return Status::OK;
}

Step::Status StepRecoverIcons::RecoveryUpdate() {
  if (!TryGatherIcons()) {
    LOG(DEBUG) << "Icons recovery not needed";
    return Status::OK;
  }
  for (auto& pair : icons_) {
    if (bf::exists(pair.first)) {
      bs::error_code error;
      bf::remove(pair.first, error);
      if (error) {
        LOG(ERROR) << "Cannot move icon to restore its correct location";
        return Status::ERROR;
      }
    }
    (void) MoveFile(pair.second, pair.first);
  }
  LOG(INFO) << "Icons recovery done";
  return Status::OK;
}

bool StepRecoverIcons::TryGatherIcons() {
  if (!context_->manifest_data.get())
    return false;
  application_x* app = nullptr;
  PKGMGR_LIST_MOVE_NODE_TO_HEAD(context_->manifest_data.get()->application,
                                app);
  for (; app != nullptr; app = app->next) {
    if (strcmp(app->component_type, "uiapp") != 0)
      continue;

    bf::path app_icon = bf::path(getIconPath(context_->uid.get()))
      / bf::path(app->appid);
    if (app->icon && app->icon->text)
      app_icon += bf::path(app->icon->text).extension();
    else
      app_icon += ".png";
    bf::path icon_backup = GetBackupPathForIconFile(app_icon);
    if (bf::exists(icon_backup) || bf::exists(app_icon))
        icons_.emplace_back(app_icon, icon_backup);
  }
  return true;
}

}  // namespace filesystem
}  // namespace common_installer

