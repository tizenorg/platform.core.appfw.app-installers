// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/tzip_interface.h"

namespace common_installer {

// int tzip_mount_zipfs(const char *zip_file, const char *mount_path);
// int tzip_unmount_zipfs(const char *mount_path);
// int tzip_is_mounted(const char *mount_path);

class TzipInterface::Pimpl {
 public:
  explicit Pimpl(const std::string& mount_path) :
    mount_path_(mount_path) { }

  bool MountZip(const std::string& zip_path) {
    return true;
  }
  bool UnMountZip() {
    return true;
  }
  bool Relocate(const std::string& new_mount_path) {
    return true;
  }

 private:
  std::string mount_path_;
  std::string zip_path_;
};


TzipInterface::TzipInterface(const std::string& mount_path)
  : impl_(new Pimpl(mount_path)) {}

TzipInterface::~TzipInterface() { }

bool TzipInterface::MountZip(const std::string& zip_path) {
  return impl_->MountZip(zip_path);
}

bool TzipInterface::UnMountZip() {
  return impl_->UnMountZip();
}

bool TzipInterface::Relocate(const std::string& new_mount_path) {
  return impl_->Relocate(new_mount_path);
}

}  // namespace common_installer
