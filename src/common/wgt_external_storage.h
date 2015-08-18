// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_WGT_EXTERNAL_STORAGE_H_
#define COMMON_WGT_EXTERNAL_STORAGE_H_

#include <app2ext_interface.h>

#include <string>

#include "wgt/wgt_backend_data.h"

namespace il = wgt::parse;

namespace common_installer {

class WgtExternalStorage {
 public:
  WgtExternalStorage();
  ~WgtExternalStorage();
  void PostInstallation(app2ext_status_t status, const char* pkgid);
  bool PreInstallation(const char* pkgid);
  void PostUninstallation(const char* pkgid);
  bool PreUninstallation(const char* pkgid);

  app2ext_handle* app2_handle_;
};
}  //  namespace common_installer

#endif  //  COMMON_WGT_EXTERNAL_STORAGE_H_
