// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/external_mount.h"

#include <app2ext_interface.h>
#include <manifest_parser/utils/logging.h>

namespace common_installer {

ExternalMount::ExternalMount(const std::string& pkgid, uid_t uid)
    : pkgid_(pkgid),
      uid_(uid),
      mounted_(false) {
}

ExternalMount::~ExternalMount() {
  if (mounted_) {
    int ret = app2ext_usr_disable_external_pkg(pkgid_.c_str(), uid_);
    if (ret < 0) {
      LOG(ERROR) << "app2ext_usr_disable_external_pkg failed with error code: "
                 << ret;
    }
  }
}

bool ExternalMount::IsAvailable() const {
  return app2ext_usr_get_app_location(pkgid_.c_str(), uid_) == APP2EXT_SD_CARD;
}

bool ExternalMount::Mount() {
  if (mounted_)
    return true;
  int ret = app2ext_usr_enable_external_pkg(pkgid_.c_str(), uid_);
  if (ret < 0) {
    LOG(ERROR) << "app2ext_usr_enable_external_pkg failed with error code: "
               << ret;
    return false;
  }
  mounted_ = true;
  return true;
}

bool ExternalMount::Umount() {
  if (!mounted_)
    return true;
  int ret = app2ext_usr_disable_external_pkg(pkgid_.c_str(), uid_);
  if (ret < 0) {
    LOG(ERROR) << "app2ext_usr_disable_external_pkg failed with error code: "
               << ret;
    return false;
  }
  mounted_ = false;
  return true;
}

}  // namespace common_installer
