// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_create_icons.h"

#include <boost/filesystem/operations.hpp>
#include <boost/system/error_code.hpp>
#include <pkgmgr-info.h>

#include "common/utils/glist_range.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace common_installer {
namespace filesystem {

Step::Status StepCreateIcons::undo() {
  for (auto& icon : icons_) {
    bs::error_code error;
    bf::remove_all(icon, error);
  }
  return Status::OK;
}

Step::Status StepCreateIcons::process() {
  bf::path destination =
      getIconPath(context_->uid.get(), context_->is_preload_request.get());
  bs::error_code error;
  if (!bf::exists(destination)) {
    bf::create_directories(destination, error);
    if (error) {
      LOG(ERROR) << "Cannot create directory of application icons: "
                 << destination;
      return Status::ERROR;
    }
  }

  for (application_x* app :
      GListRange<application_x*>(context_->manifest_data.get()->application)) {
    // TODO(t.iwanek): this ignores icon locale as well in same way as other
    // steps -> icons should be localized
    if (app->icon) {
      icon_x* icon = reinterpret_cast<icon_x*>(app->icon->data);
      bf::path source(icon->text);
      if (bf::exists(source)) {
        bf::path destination_path = destination / app->appid;
        if (source.has_extension())
          destination_path += source.extension();
        else
          destination_path += ".png";
        bf::copy_file(source, destination_path, error);
        if (error) {
          LOG(ERROR) << "Cannot create package icon: " << destination_path
                     << " , error: " << error;
          return Status::ICON_ERROR;
        }
        icons_.push_back(destination_path);
      }
    }
  }
  LOG(DEBUG) << "Icon files created";
  return Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer
