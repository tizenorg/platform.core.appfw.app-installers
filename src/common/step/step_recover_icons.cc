// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_recover_icons.h"

#include <boost/filesystem/operations.hpp>
#include <boost/system/error_code.hpp>
#include <pkgmgr-info.h>

#include "common/backup_paths.h"
#include "common/utils/file_util.h"
#include "common/utils/glist_range.h"

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
        return Status::RECOVERY_ERROR;
      }
    }
    (void) MoveFile(pair.second, pair.first);
  }
  LOG(INFO) << "Icons recovery done";
  return Status::OK;
}

std::vector<boost::filesystem::path> StepRecoverIcons::GetIconsPaths() {
  std::vector<bf::path> paths;
  paths.push_back(bf::path(getIconPath(context_->uid.get())));
  return paths;
}

bool StepRecoverIcons::TryGatherIcons() {
  if (!context_->manifest_data.get())
    return false;
  for (application_x* app :
       GListRange<application_x*>(context_->manifest_data.get()->application)) {
    for (const auto& path : GetIconsPaths()) {
      bf::path icon_path = path / bf::path(app->appid);
      if (app->icon) {
        icon_x* icon = reinterpret_cast<icon_x*>(app->icon->data);
        icon_path += bf::path(icon->text).extension();
      } else {
        icon_path += ".png";
      }
      bf::path icon_backup = GetBackupPathForIconFile(icon_path);
      if (bf::exists(icon_backup) || bf::exists(icon_path))
          icons_.emplace_back(icon_path, icon_backup);
    }
  }
  return true;
}

}  // namespace filesystem
}  // namespace common_installer

