// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_create_storage_directories.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace {

const char kDataLocation[] = "data";
const char kSharedLocation[] = "shared";

bool CreateStorageDirectoriesForPath(const bf::path& path) {
  bs::error_code error_code;
  bf::path data_path = path / kDataLocation;
  bf::create_directory(data_path, error_code);
  if (error_code) {
    LOG(ERROR) << "Failed to create private directory for widget";
    return false;
  }
  bf::path shared_path = path / kSharedLocation;
  bf::create_directory(shared_path, error_code);
  if (error_code) {
    LOG(ERROR) << "Failed to create shared directory for widget";
    return false;
  }
  return true;
}

}  // namespace

namespace common_installer {
namespace create_storage {

common_installer::Step::Status StepCreateStorageDirectories::process() {
  if (!CreateStorageDirectoriesForPath(context_->pkg_path.get()))
    return Status::ERROR;
  return Status::OK;
}

}  // namespace create_storage
}  // namespace common_installer
