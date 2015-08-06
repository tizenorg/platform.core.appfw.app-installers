// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_WGT_INSTALLER_H_
#define WGT_WGT_INSTALLER_H_

#include "common/app_installer.h"

namespace wgt {

/**
 * @brief The WgtInstaller class
 *        Subclass of AppInstaller class dedicated for handling wgt request
 *
 * Performs all types of requests of wgt packages and sets required sequence of
 * steps.
 */
class WgtInstaller : public common_installer::AppInstaller {
 public:
  explicit WgtInstaller(common_installer::PkgMgrPtr pkgrmgr);
};

}  // namespace wgt

#endif  // WGT_WGT_INSTALLER_H_
