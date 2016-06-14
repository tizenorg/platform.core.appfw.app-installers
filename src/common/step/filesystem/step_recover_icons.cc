// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_recover_icons.h"

#include <boost/filesystem/operations.hpp>
#include <boost/system/error_code.hpp>
#include <pkgmgr-info.h>

#include "common/paths.h"
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

bool StepRecoverIcons::TryGatherIcons() {
  if (!context_->manifest_data.get())
    return false;

  // gather icon info
  const char *extra_icon_path = getIconPath(context_->uid.get(),
      context_->is_preload_request.get());
  if (!extra_icon_path)
    return true;
  for (auto iter = bf::directory_iterator(extra_icon_path);
      iter != bf::directory_iterator(); ++iter) {
    if (!bf::is_regular_file(iter->path()))
      continue;
    for (application_x* app : GListRange<application_x*>(
        context_->manifest_data.get()->application)) {
      if (app->icon) {
        bf::path p = iter->path();
        std::string filename = iter->path().filename().string();
        if (filename.find(app->appid) == 0) {
          bf::path icon;
          bf::path icon_backup;
          if (p.extension() == GetIconFileBackupExtension()) {
            icon_backup = p;
            icon = p.replace_extension("");
          } else {
            icon = p;
            icon_backup = GetBackupPathForIconFile(iter->path());
          }
          icons_.insert(std::make_pair(icon, icon_backup));
        }
      }
    }
  }
  return true;
}

}  // namespace filesystem
}  // namespace common_installer

