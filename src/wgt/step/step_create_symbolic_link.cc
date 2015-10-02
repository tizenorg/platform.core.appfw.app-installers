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
namespace filesystem {

namespace bf = boost::filesystem;

common_installer::Step::Status StepCreateSymbolicLink::process() {
  assert(context_->manifest_data.get());
  boost::system::error_code error;
  application_x* app = context_->manifest_data.get()->application;
  if (!app) {
     LOG(ERROR) << "There is no application described!";
    return Step::Status::ERROR;
  }
  // add ui-application element per ui application
  for (; app != nullptr; app = app->next) {
    // binary is a symbolic link named <appid> and is located in <pkgid>/<appid>
    bf::path exec_path = context_->package_storage->path() / "bin";
    common_installer::CreateDir(exec_path);

    exec_path /= bf::path(app->appid);

    bf::create_symlink(bf::path(WRT_LAUNCHER), exec_path, error);
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
  application_x* app = context_->manifest_data.get()->application;

  for (; app != nullptr; app = app->next) {
    bf::path exec_path = context_->package_storage->path() / "bin";
    if (bf::exists(exec_path))
      bf::remove_all(exec_path);
  }
  return Status::OK;
}

}  // namespace filesystem
}  // namespace wgt
