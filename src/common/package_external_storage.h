// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_PACKAGE_EXTERNAL_STORAGE_H_
#define COMMON_PACKAGE_EXTERNAL_STORAGE_H_

#include <app2ext_interface.h>
#include <boost/filesystem/path.hpp>

#include <string>

#include "common/package_storage.h"

namespace common_installer {

class PackageExternalStorage : public PackageStorage {
 public:
  PackageExternalStorage(RequestType type, const std::string& pkgid,
                         const boost::filesystem::path& application_root);
  ~PackageExternalStorage() override;
  bool Initialize(const boost::filesystem::path& space_requirement) override;
  bool Commit() override;
  bool Abort() override;
  const std::string& StorageName() const override;

 private:
  bool Finalize(bool success);

  app2ext_handle* handle_;
};

}  // namespace common_installer

#endif  // COMMON_PACKAGE_EXTERNAL_STORAGE_H_
