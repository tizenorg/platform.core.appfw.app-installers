// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_EXTERNAL_STORAGE_H_
#define COMMON_EXTERNAL_STORAGE_H_

#include <boost/filesystem/path.hpp>

#include <app2ext_interface.h>
#include <sys/types.h>

#include <string>

#include "common/request.h"

namespace common_installer {

class ExternalStorage final {
 public:
  static std::unique_ptr<ExternalStorage> AcquireExternalStorage(
      RequestType type, const boost::filesystem::path& application_root,
      const std::string& pkgid,
      const boost::filesystem::path& space_requirement, uid_t uid);

  ExternalStorage(RequestType type, const std::string& pkgid,
                  const boost::filesystem::path& application_root,  uid_t uid);
  ~ExternalStorage();

  bool Commit();
  bool Abort();

 private:
  bool Initialize(const boost::filesystem::path& space_requirement);
  bool Finalize(bool success);

  RequestType type_;
  std::string pkgid_;
  boost::filesystem::path application_root_;
  uid_t uid_;
  app2ext_handle* handle_;
};

}  // namespace common_installer

#endif  // COMMON_EXTERNAL_STORAGE_H_
