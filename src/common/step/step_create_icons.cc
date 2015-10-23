// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_create_icons.h"

#include <boost/filesystem/operations.hpp>
#include <boost/system/error_code.hpp>
#include <pkgmgr-info.h>

#include "common/utils/glist_range.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace common_installer {
namespace filesystem {

Step::Status StepCreateIcons::process() {
  bf::path icons_directory(getIconPath(context_->uid.get()));
  if (!bf::exists(icons_directory)) {
    bs::error_code error;
    bf::create_directories(icons_directory, error);
    if (error) {
      LOG(ERROR) << "Cannot create directory of application icons";
      return Status::ERROR;
    }
  }

  for (application_x* app :
       GListRange<application_x*>(context_->manifest_data.get()->application)) {
    // TODO(t.iwanek): this is ignoring icon locale as well as other steps
    // icons should be localized
    if (app->icon) {
      icon_x* icon = reinterpret_cast<icon_x*>(app->icon->data);
      bf::path source = GetIconRoot() / icon->text;
      if (bf::exists(source)) {
        bf::path destination = icons_directory / app->appid;
        if (source.has_extension())
          destination += source.extension();
        else
          destination += ".png";
        bs::error_code error;
        bf::copy_file(source, destination, error);
        if (error) {
          LOG(ERROR) << "Cannot create package icon: " << destination;
          return Status::ERROR;
        }
        icons_.push_back(destination);
      }
    }
  }
  LOG(DEBUG) << "Icon files created";
  return Status::OK;
}

Step::Status StepCreateIcons::undo() {
  for (auto& icon : icons_) {
    bs::error_code error;
    bf::remove_all(icon, error);
  }
  return Status::OK;
}

boost::filesystem::path StepCreateIcons::GetIconRoot() const {
  // TODO(t.iwanek): shared/res is location of icons for tpk
  return context_->pkg_path.get() / "shared" / "res";
}

}  // namespace filesystem
}  // namespace common_installer
