// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/package_internal_storage.h"

#include <cassert>

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace common_installer {

bool PackageInternalStorage::Commit() {
  return true;
}

bool PackageInternalStorage::Abort() {
  return true;
}

bool PackageInternalStorage::Initialize(
    const boost::filesystem::path& /*space_requirement*/) {
  // Space requirement for internal installation is checked when package is
  // unzipped
  return true;
}

bool PackageInternalStorage::IsRequestedPackageInstalled() const {
  assert(false && "Not implemented");
}

}  // namespace common_installer
