// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_create_storage_directories.h"

#include <manifest_parser/utils/version_number.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>

#include <string>
#include <vector>

#include "common/shared_dirs.h"
#include "common/utils/glist_range.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace {

const char kCache[] = "cache";
const char kData[] = "data";
const char kShared[] = "shared";
const char kSharedCache[] = "shared/cache";
const char kSharedData[] = "shared/data";
const char kSharedTrusted[] = "shared/trusted";
const utils::VersionNumber ver30("3.0");

}  // namespace

namespace common_installer {
namespace filesystem {

common_installer::Step::Status StepCreateStorageDirectories::process() {
  if (context_->request_mode.get() == RequestMode::GLOBAL) {
    // remove packaged RW diriectories
    RemoveAll();
    return Status::OK;
  }
  if (!ShareDir())
    return Status::APP_DIR_ERROR;
  if (!PrivateDir())
    return Status::APP_DIR_ERROR;
  if (!CacheDir())
    return Status::APP_DIR_ERROR;
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

  if (!SubShareDir())
    return false;

  return true;
}

bool StepCreateStorageDirectories::SubShareDir() {
  bs::error_code error_code;
  bf::path shared_trusted_path = context_->pkg_path.get() / kSharedTrusted;
  bf::create_directory(shared_trusted_path, error_code);
  if (error_code) {
    LOG(ERROR) << "Failed to create shared/trusted directory for package";
    return false;
  }

  manifest_x* manifest = context_->manifest_data.get();
  if (!manifest) {
    LOG(ERROR) << "Failed to get manifest info";
    return false;
  }
  std::string str_ver(manifest->api_version);
  utils::VersionNumber api_version(str_ver);

  bf::path shared_data_path = context_->pkg_path.get() / kSharedData;
  bf::path shared_cache_path = context_->pkg_path.get() / kSharedCache;

  if (api_version >= ver30) {
    // remove shared/data (deprecated)
    bf::remove_all(shared_data_path, error_code);
    if (error_code) {
      LOG(ERROR) << "Can't remove dir:" << shared_data_path;
      return false;
    }
  } else {
    bf::create_directory(shared_data_path, error_code);
    if (error_code) {
      LOG(ERROR) << "Failed to create shared/data directory for package";
      return false;
    }
  }

  // remove shared/cache (do not support)
  bf::remove_all(shared_cache_path, error_code);
  if (error_code) {
    LOG(ERROR) << "Can't remove dir:" << shared_cache_path;
    return false;
  }

  return true;
}

bool StepCreateStorageDirectories::PrivateDir() {
  bs::error_code error_code;
  bf::path data_path = context_->pkg_path.get() / kData;

  // compatibility for old tpk packages
  if (bf::exists(data_path)) {
    LOG(DEBUG) << "Data directory already exists";
    return true;
  }

  bf::create_directory(data_path, error_code);
  if (error_code) {
    LOG(ERROR) << "Failed to create private directory for package";
    return false;
  }
  return true;
}

bool StepCreateStorageDirectories::CacheDir() {
  bs::error_code error_code;
  bf::path cache_path = context_->pkg_path.get() / kCache;
  bf::create_directory(cache_path, error_code);
  if (error_code) {
    LOG(ERROR) << "Failed to create cache directory for package";
    return false;
  }
  return true;
}

void StepCreateStorageDirectories::RemoveAll() {
  bs::error_code error_code;
  bf::path data_path = context_->pkg_path.get() / kData;
  bf::path cache_path = context_->pkg_path.get() / kCache;
  bf::path shared_data_path = context_->pkg_path.get() / kSharedData;
  bf::path shared_cache_path = context_->pkg_path.get() / kSharedCache;
  bf::path shared_trusted_path = context_->pkg_path.get() / kSharedTrusted;

  bf::remove_all(data_path, error_code);
  bf::remove_all(cache_path, error_code);
  bf::remove_all(shared_data_path, error_code);
  bf::remove_all(shared_cache_path, error_code);
  bf::remove_all(shared_trusted_path, error_code);
}

}  // namespace filesystem
}  // namespace common_installer
