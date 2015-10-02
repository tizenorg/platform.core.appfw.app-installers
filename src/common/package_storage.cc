// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/package_storage.h"

#include <boost/filesystem/operations.hpp>
#include <boost/system/error_code.hpp>

#include <manifest_parser/utils/logging.h>

#include <cstring>
#include <list>

#include "common/package_external_storage.h"
#include "common/package_internal_storage.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace {

const char kPreferExternal[] = "prefer-external";
const char kInternalOnly[] = "internal-only";

}

namespace common_installer {

boost::filesystem::path PackageStorage::path() const {
  return application_root_ / pkgid_;
}

// Implements policy of acquiring package location
std::unique_ptr<PackageStorage> AcquirePackageStorage(RequestType type,
    const boost::filesystem::path& application_root, const std::string& pkgid,
    const boost::filesystem::path& space_requirement,
    const char* installed_storage, const char* preference, uid_t uid) {
  LOG(INFO) << "Acquiring package storage with preference: '" << preference
            << "' and current installation: '"
            << (installed_storage ?
                std::string(installed_storage) : std::string("none")) << "'";
  std::unique_ptr<PackageStorage> external_storage(
      new PackageExternalStorage(type, pkgid, application_root, uid));
  std::unique_ptr<PackageStorage> internal_storage(
      new PackageInternalStorage(type, pkgid, application_root, uid));

  // if installed
  if (installed_storage) {
    if (external_storage->StorageName() == installed_storage) {
      if (!external_storage->Initialize(space_requirement)) {
        LOG(ERROR) << "Initialization of external storage failed";
        return nullptr;
      }
      return external_storage;
    } else if (internal_storage->StorageName() == installed_storage) {
      if (!internal_storage->Initialize(space_requirement)) {
        LOG(ERROR) << "Initialization of internal storage failed";
        return nullptr;
      }
      return internal_storage;
    } else {
      return nullptr;
    }
  }

  // if not installed
  if (preference) {
    if (strcmp(preference, kPreferExternal) == 0) {
      if (external_storage->Initialize(space_requirement)) {
        return external_storage;
      }
      LOG(WARNING) << "Initialization of external storage failed";
    }
    if (!internal_storage->Initialize(space_requirement)) {
      if (strcmp(preference, kInternalOnly) == 0) {
        LOG(ERROR) << "Initialization of internal storage failed";
        return nullptr;
      }
      LOG(WARNING) << "Initialization of internal storage failed";
      if (external_storage->Initialize(space_requirement)) {
        return external_storage;
      }
      LOG(WARNING) << "Initialization of external storage failed";
    }
    return internal_storage;
  }

  return nullptr;
}

}  // namespace common_installer
