// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a APACHE-2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_WGT_INSTALLER_H_
#define WGT_WGT_INSTALLER_H_

#include "common/app_installer.h"

class WgtInstaller : public common_installer::AppInstaller {
 public:
  WgtInstaller(int request, const char* file, const char* pkgid);
};

#endif  // WGT_WGT_INSTALLER_H_
