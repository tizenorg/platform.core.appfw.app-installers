// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.


#include "common/step/step_remove_files.h"

#include <pkgmgr-info.h>

#include <cstring>

#include "common/utils/file_util.h"

namespace common_installer {
namespace remove {

namespace fs = boost::filesystem;

Step::Status StepRemoveFiles::precheck() {
  if (!context_->manifest_data.get()) {
    LOG(ERROR) << "manifest_data attribute is empty";
    return Step::Status::INVALID_VALUE;
  }

  // Even though, the below checks can fail, StepRemoveFiles should still try
  // to remove the files
  if (context_->pkg_path.get().empty())
    LOG(ERROR) << "pkg_path attribute is empty";
  else if (!fs::exists(context_->pkg_path.get()))
    LOG(ERROR) << "pkg_path ("
               << context_->pkg_path.get()
               << ") path does not exist";

  if (context_->xml_path.get().empty())
    LOG(ERROR) << "xml_path attribute is empty";
  else if (!fs::exists(context_->xml_path.get()))
    LOG(ERROR) << "xml_path ("
               << context_->xml_path.get()
               << ") path does not exist";

  // TODO(p.sikorski) check context_->uid.get()

  return Step::Status::OK;
}

Step::Status StepRemoveFiles::process() {
  uiapplication_x* ui = context_->manifest_data.get()->uiapplication;

  fs::remove_all(context_->pkg_path.get());
  for (; ui != nullptr; ui = ui->next) {
    fs::path app_icon = fs::path(getIconPath(context_->uid.get()))
      / fs::path(ui->appid);
    app_icon += fs::path(".png");
    if (fs::exists(app_icon))
      fs::remove_all(app_icon);
  }
  fs::remove_all(context_->xml_path.get());

  LOG(DEBUG) << "Removing dir: " << context_->pkg_path.get();

  return Status::OK;
}

Step::Status StepRemoveFiles::clean() {
  return Status::OK;
}

Step::Status StepRemoveFiles::undo() {
  return Status::OK;
}

}  // namespace remove
}  // namespace common_installer
