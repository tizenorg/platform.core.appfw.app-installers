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
  std::vector<bf::path> paths { getIconPath(context_->uid.get()) };
  return CopyIcons(paths);
}

Step::Status StepCreateIcons::undo() {
  for (auto& icon : icons_) {
    bs::error_code error;
    bf::remove_all(icon, error);
  }
  return Status::OK;
}

Step::Status StepCreateIcons::CopyIcons(
    const std::vector<bf::path>& destinations) {
  for (application_x* app :
      GListRange<application_x*>(context_->manifest_data.get()->application)) {
    if (GetAppTypeForIcons() != app->type)
      continue;
    // TODO(t.iwanek): this is ignoring icon locale as well as other steps
    // icons should be localized
    if (app->icon) {
      icon_x* icon = reinterpret_cast<icon_x*>(app->icon->data);
      bf::path source = GetIconRoot() / icon->text;
      if (bf::exists(source)) {
        for (const auto& destination : destinations) {
          bs::error_code error;
          if (!bf::exists(destination)) {
            bf::create_directories(destination, error);
            if (error) {
              LOG(ERROR) << "Cannot create directory of application icons: "
                  << destination;
              return Status::ERROR;
            }
          }
          bf::path destination_path = destination / app->appid;
          if (source.has_extension())
            destination_path += source.extension();
          else
            destination_path += ".png";
          bf::copy_file(source, destination_path, error);
          if (error) {
            LOG(ERROR) << "Cannot create package icon: " << destination_path;
            return Status::ICON_ERROR;
          }
          icons_.push_back(destination_path);
        }
      }
    }
  }
  LOG(DEBUG) << "Icon files created";
  return Status::OK;
}

boost::filesystem::path StepCreateIcons::GetIconRoot() const {
  // TODO(t.iwanek): shared/res is location of icons for tpk
  return context_->pkg_path.get() / "shared" / "res";
}

std::string StepCreateIcons::GetAppTypeForIcons() const {
  return "capp";
}

}  // namespace filesystem
}  // namespace common_installer
