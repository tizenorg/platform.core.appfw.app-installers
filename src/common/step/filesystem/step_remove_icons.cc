// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_remove_icons.h"

#include <boost/filesystem/operations.hpp>
#include <boost/system/error_code.hpp>
#include <pkgmgr-info.h>

#include <cstring>
#include <string>

#include "common/utils/glist_range.h"

namespace bs = boost::system;
namespace bf = boost::filesystem;


namespace common_installer {
namespace filesystem {

Step::Status StepRemoveIcons::precheck() {
  if (!context_->manifest_data.get()) {
    LOG(ERROR) << "manifest_data attribute is empty";
    return Status::MANIFEST_NOT_FOUND;
  }

  return Status::OK;
}

Step::Status StepRemoveIcons::process() {
  for (auto iter = bf::directory_iterator(getIconPath(context_->uid.get(),
          context_->is_preload_request.get()));
      iter != bf::directory_iterator(); ++iter) {
    if (!bf::is_regular_file(iter->path()))
      continue;
    bs::error_code error;
    for (application_x* app :
        GListRange<application_x*>(
           context_->manifest_data.get()->application)) {
      if (app->icon) {
        std::string filename = iter->path().filename().string();
        if (filename.find(app->appid) == 0) {
          bf::remove(iter->path(), error);
          if (error) {
            LOG(WARNING) << "Failed to remove: " << filename;
          }
        }
      }
    }
  }
  return Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer
