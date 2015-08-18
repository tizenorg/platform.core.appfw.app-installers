// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_WGT_EXTERNAL_STORAGE_H_
#define COMMON_WGT_EXTERNAL_STORAGE_H_

#include <app2ext_interface.h>

#include <string>

namespace common_installer {

class WgtExternalStorage {
 public:
  WgtExternalStorage();
  ~WgtExternalStorage();
  bool IsInitialized();
  bool PostInstallation(bool execute, const std::string& pkgid);
  bool PreInstallation(const std::string& pkgid);
  bool PostUninstallation(const std::string& pkgid);
  bool PreUninstallation(const std::string& pkgid);

 private:
  app2ext_handle* app2_handle_;
  bool initialization_;
  app2ext_status_t status;
  std::string pkgid_;
};
}  //  namespace common_installer

#endif  //  COMMON_WGT_EXTERNAL_STORAGE_H_
