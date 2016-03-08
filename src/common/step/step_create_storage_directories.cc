// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_create_storage_directories.h"

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
const char kSharedData[] = "data";
const char kSharedTrusted[] = "trusted";

}  // namespace

namespace common_installer {
namespace filesystem {

common_installer::Step::Status StepCreateStorageDirectories::process() {
  if (context_->request_mode.get() == RequestMode::GLOBAL)
    return Status::OK;
  if (!ShareDir())
    return Status::APP_DIR_ERROR;
  if (!PrivateDir())
    return Status::APP_DIR_ERROR;
  if (!CacheDir())
    return Status::APP_DIR_ERROR;
  if (!ExtStorageDir())
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

bool StepCreateStorageDirectories::ExtStorageDir() {
  auto manifest = context_->manifest_data.get();
  std::vector<std::string> priv_vec;
  for (const char* priv : GListRange<char*>(manifest->privileges)) {
    priv_vec.emplace_back(priv);
  }

  if (std::find(priv_vec.begin(), priv_vec.end(), "externalstorage.appdata") ==
      priv_vec.end()) return true;

  auto pkg_list = CreatePkgInformationList({ context_->pkgid.get()});
  if (pkg_list.empty()) return false;

  pkg_info pkg = *pkg_list.begin();

  switch (context_->request_mode.get()) {
    case RequestMode::GLOBAL: {
      if (!PerformExternalDirectoryCreationForAllUsers(pkg.pkg_id,
                                                       pkg.author_id,
                                                       pkg.api_version))
        return false;
    }
    break;
    case RequestMode::USER: {
      if (!PerformExternalDirectoryCreationForUser(getuid(),
                                                   pkg.pkg_id,
                                                   pkg.author_id,
                                                   pkg.api_version))
        return false;
    }
    break;
  }

  return true;
}


}  // namespace filesystem
}  // namespace common_installer
