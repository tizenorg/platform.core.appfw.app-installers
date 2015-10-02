// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/package_storage.h"

#include <boost/filesystem/operations.hpp>
#include <boost/system/error_code.hpp>

#include <cstring>
#include <list>

#include "common/package_external_storage.h"
#include "common/package_internal_storage.h"
#include "common/utils/logging.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace {

const char kPreferExternal[] = "prefer-external";

}

namespace common_installer {

boost::filesystem::path PackageStorage::path() const {
  return application_root_ / pkgid_;
}

std::unique_ptr<PackageStorage> CreatePackageStorage(RequestType type,
    const boost::filesystem::path& application_root, const std::string& pkgid,
    const boost::filesystem::path& space_requirement,
    const char* install_location) {
  bool try_internal = true;
  bool try_external = false;
  std::unique_ptr<PackageStorage> external_storage;

  switch (type) {
  case RequestType::Install: {
    if (strcmp(install_location, kPreferExternal) == 0) {
      try_external = true;
      external_storage.reset(
          new PackageExternalStorage(type, pkgid, application_root));
    }
    break;
  }
  case RequestType::Update: {
    external_storage.reset(
        new PackageExternalStorage(type, pkgid, application_root));
    if (external_storage->IsRequestedPackageInstalled()) {
      try_external = true;
      try_internal = false;
    }
    break;
  }
  case RequestType::Uninstall: {
    external_storage.reset(
        new PackageExternalStorage(type, pkgid, application_root));
    if (external_storage->IsRequestedPackageInstalled()) {
      try_external = true;
      try_internal = false;
    }
    break;
  }
  case RequestType::Reinstall: {
    external_storage.reset(
        new PackageExternalStorage(type, pkgid, application_root));
    if (external_storage->IsRequestedPackageInstalled()) {
      LOG(ERROR) << "Reinstall mode is not supported for external installation";
      try_internal = false;
    }
    break;
  }
  case RequestType::Recovery: {
    external_storage.reset(
        new PackageExternalStorage(type, pkgid, application_root));
    if (external_storage->IsRequestedPackageInstalled()) {
      LOG(ERROR) << "Recovery mode is not supported for external installation";
      try_internal = false;
    }
    break;
  }
  default: {
    assert(false && "Invalid installation mode");
  }
  }

  if (try_external) {
    if (external_storage->Initialize(space_requirement)) {
      return external_storage;
    }
    LOG(WARNING) << "Initialization of external filesystem failed";
    external_storage.reset();
  }

  if (try_internal) {
    std::unique_ptr<PackageStorage> internal_storage(
        new PackageInternalStorage(type, pkgid, application_root));
    if (!internal_storage->Initialize(space_requirement)) {
      LOG(ERROR) << "Initialization of internal filesystem failed";
      return nullptr;
    }
    return internal_storage;
  }
  return nullptr;
}

}  // namespace common_installer
