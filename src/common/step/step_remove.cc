// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.


#include "common/step/step_remove.h"

#include <pkgmgr-info.h>

#include <cstring>

#include "utils/file_util.h"

namespace common_installer {
namespace remove {

namespace fs = boost::filesystem;

Step::Status StepRemove::process() {
  uiapplication_x* ui = context_->manifest_data()->uiapplication;

  if (!fs::exists(context_->pkg_path()))
    LOG(DEBUG) << "dir: " << context_->pkg_path() << "not exist";

  fs::remove_all(context_->pkg_path());
  for (; ui != nullptr; ui = ui->next) {
    fs::path app_icon = fs::path(getIconPath(context_->uid()))
      / fs::path(ui->appid);
    app_icon += fs::path(".png");
    if (fs::exists(app_icon))
      fs::remove_all(app_icon);
  }
  fs::remove_all(context_->xml_path());

  LOG(DEBUG) << "Removing dir: " << context_->pkg_path();

  return Status::OK;
}

Step::Status StepRemove::clean() {
  return Status::OK;
}

Step::Status StepRemove::undo() {
  return Status::OK;
}

}  // namespace remove
}  // namespace common_installer
