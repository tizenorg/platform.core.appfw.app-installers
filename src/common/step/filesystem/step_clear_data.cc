// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_clear_data.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace {

const char kDataDirectory[] = "data";

}  // namespace

namespace common_installer {
namespace filesystem {

Step::Status StepClearData::precheck() {
  if (context_->pkgid.get().empty()) {
    LOG(ERROR) << "Pkgid is not set";
    return Status::INVALID_VALUE;
  }
  return Status::OK;
}

Step::Status StepClearData::process() {
  Status status = Status::OK;
  context_->pkg_path.set(
      context_->root_application_path.get() / context_->pkgid.get());
  bf::path data_directory = context_->pkg_path.get() / kDataDirectory;
  for (auto iter = bf::directory_iterator(data_directory);
      iter != bf::directory_iterator(); ++iter) {
    bs::error_code error;
    bf::remove_all(iter->path(), error);
    if (error) {
      LOG(ERROR) << "Failed to remove: " << iter->path();
      status = Status::APP_DIR_ERROR;
    }
  }
  LOG(DEBUG) << "Removing files from " << data_directory << " finished";
  return status;
}

}  // namespace filesystem
}  // namespace common_installer

