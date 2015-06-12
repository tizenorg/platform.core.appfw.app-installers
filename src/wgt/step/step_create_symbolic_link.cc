/* 2014, Copyright © Eurogiciel Coporation, APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_create_symbolic_link.h"

#include <pkgmgr-info.h>
#include <unistd.h>

#include <boost/filesystem.hpp>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <string>

#include "common/utils/file_util.h"

namespace wgt {
namespace symbolic_link {

namespace fs = boost::filesystem;

common_installer::Step::Status StepCreateSymbolicLink::process() {
  assert(context_->manifest_data.get());
  boost::system::error_code error;
  uiapplication_x* ui = context_->manifest_data.get()->uiapplication;
  serviceapplication_x* svc =
      context_->manifest_data.get()->serviceapplication;
  if ((!ui) && (!svc)) {
     LOG(ERROR) << "There is neither UI applications nor"
        << "Services applications described!";
    return Step::Status::ERROR;
  }
  // add ui-application element per ui application
  for (; ui != nullptr; ui = ui->next) {
    // binary is a symbolic link named <appid> and is located in <pkgid>/<appid>
    fs::path exec_path =
        context_->pkg_path.get()
            / fs::path("bin");
    common_installer::CreateDir(exec_path);

    exec_path /= fs::path(ui->appid);

    fs::create_symlink(fs::path(WRT_LAUNCHER), exec_path, error);
    if (error) {
      LOG(ERROR) << "Failed to set symbolic link "
        << boost::system::system_error(error).what();
      return Step::Status::ERROR;
    }
  }
  for (; svc != nullptr; svc = svc->next) {
    // binary is a symbolic link named <appid> and is located in <pkgid>/<appid>
    fs::path exec_path = context_->pkg_path.get() / fs::path("bin");
    common_installer::CreateDir(exec_path);

    exec_path /= fs::path(svc->appid);

    fs::create_symlink(fs::path(WRT_LAUNCHER), exec_path, error);
    if (error) {
      LOG(ERROR) << "Failed to set symbolic link "
        << boost::system::system_error(error).what();
      return Step::Status::ERROR;
    }
  }
  LOG(DEBUG) << "Successfully parse tizen manifest xml";

  return Status::OK;
}

common_installer::Step::Status StepCreateSymbolicLink::undo() {
  uiapplication_x* ui = context_->manifest_data.get()->uiapplication;
  serviceapplication_x* svc = context_->manifest_data.get()->serviceapplication;

  for (; ui != nullptr; ui = ui->next) {
    fs::path exec_path = context_->pkg_path.get() / fs::path("bin");
    if (fs::exists(exec_path))
      fs::remove_all(exec_path);
  }
  for (; svc != nullptr; svc = svc->next) {
    fs::path exec_path = context_->pkg_path.get() / fs::path("bin");
    if (fs::exists(exec_path))
      fs::remove_all(exec_path);
  }
  return Status::OK;
}

}  // namespace symbolic_link
}  // namespace wgt
