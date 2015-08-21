// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_create_icons.h"

#include <boost/filesystem/operations.hpp>
#include <boost/system/error_code.hpp>
#include <pkgmgr-info.h>

#include "common/utils/clist_helpers.h"

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

  uiapplication_x* ui = nullptr;
  PKGMGR_LIST_MOVE_NODE_TO_HEAD(context_->manifest_data.get()->uiapplication,
                                ui);
  for (; ui; ui = ui->next) {
    // TODO(t.iwanek): this is ignoring icon locale as well as other steps
    // icons should be localized
    if (ui->icon && ui->icon->text) {
      bf::path source = GetIconRoot() / ui->icon->text;
      if (bf::exists(source)) {
        bf::path destination = icons_directory / ui->appid;
        if (destination.has_extension())
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
  return context_->pkg_path.get();
}

}  // namespace filesystem
}  // namespace common_installer
