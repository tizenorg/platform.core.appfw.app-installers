// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_PACKAGE_INTERNAL_STORAGE_H_
#define COMMON_PACKAGE_INTERNAL_STORAGE_H_

#include <boost/filesystem/path.hpp>

#include <string>

#include "common/package_storage.h"
#include "common/request.h"

namespace common_installer {

class PackageInternalStorage : public PackageStorage {
 public:
  using PackageStorage::PackageStorage;
  bool Initialize(const boost::filesystem::path& space_requirement) override;
  bool IsRequestedPackageInstalled() const override;
  bool Commit() override;
  bool Abort() override;
  const std::string& StorageName() const override;
};

}  // namespace common_installer

#endif  // COMMON_PACKAGE_INTERNAL_STORAGE_H_
