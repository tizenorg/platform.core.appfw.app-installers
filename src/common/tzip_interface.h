// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_TZIP_INTERFACE_H_
#define COMMON_TZIP_INTERFACE_H_

#include <memory>
#include <string>

namespace common_installer {

class TzipInterface {
 public:
  explicit TzipInterface(const std::string& mount_path);
  ~TzipInterface();

  bool MountZip(const std::string& zip_path);
  bool UnMountZip();

 private:
  class Pimpl;
  std::unique_ptr<Pimpl> impl_;
};
}  // namespace common_installer

#endif  // COMMON_TZIP_INTERFACE_H_
