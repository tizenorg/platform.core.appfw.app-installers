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

const char kData[] = "data";
const char kShared[] = "shared";
const char kSharedData[] = "data";
const char kSharedTrusted[] = "trusted";

}  // namespace

namespace common_installer {
namespace create_storage {

common_installer::Step::Status StepCreateStorageDirectories::process() {
  if (!ShareDir())
    return Status::ERROR;
  if (!SubShareDir())
    return Status::ERROR;
  if (!PrivateDir())
    return Status::ERROR;

  return Status::OK;
}

bool StepCreateStorageDirectories::ShareDir() {
  bs::error_code error_code;
  bf::path shared_path = context_->pkg_path.get() / kShared;
  bf::create_directory(shared_path, error_code);
  if (error_code) {
    LOG(ERROR) << "Failed to create shared directory for package";
    return false;
  }

  bf::path shared_trusted_path = shared_path / kSharedTrusted;
  bf::create_directory(shared_trusted_path, error_code);
  if (error_code) {
    LOG(ERROR) << "Failed to create shared/trusted directory for package";
    return false;
  }

  bf::path shared_data_path = shared_path / kSharedData;
  bf::create_directory(shared_data_path, error_code);
  if (error_code) {
    LOG(ERROR) << "Failed to create shared/data directory for package";
    return false;
  }

  return true;
}
bool StepCreateStorageDirectories::SubShareDir() {
  bs::error_code error_code;
  bf::path shared_path = context_->pkg_path.get() / kShared;
  bf::path shared_trusted_path = shared_path / kSharedTrusted;
  bf::create_directory(shared_trusted_path, error_code);
  if (error_code) {
    LOG(ERROR) << "Failed to create shared/trusted directory for package";
    return false;
  }

  bf::path shared_data_path = shared_path / kSharedData;
  bf::create_directory(shared_data_path, error_code);
  if (error_code) {
    LOG(ERROR) << "Failed to create shared/data directory for package";
    return false;
  }

  return true;
}

bool StepCreateStorageDirectories::PrivateDir() {
  bs::error_code error_code;
  bf::path data_path = context_->pkg_path.get() / kData;
  bf::create_directory(data_path, error_code);
  if (error_code) {
    LOG(ERROR) << "Failed to create private directory for package";
    return false;
  }
  return true;
}


}  // namespace create_storage
}  // namespace common_installer
