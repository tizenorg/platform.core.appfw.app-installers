// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_EXTERNAL_STORAGE_H_
#define COMMON_EXTERNAL_STORAGE_H_

#include <app2ext_interface.h>

#include <string>

namespace common_installer {

class ExternalStorage {
 public:
  explicit ExternalStorage(const std::string& pkgid);
  ~ExternalStorage();
  bool IsInitialized();
  bool PostInstallation(bool execute);
  bool PreInstallation();
  bool PostUninstallation();
  bool PreUninstallation();

 private:
  app2ext_handle* app2_handle_;
  bool initialization_;
  std::string pkgid_;
};
}  //  namespace common_installer

#endif  //  COMMON_EXTERNAL_STORAGE_H_
