// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/request.h"
#include <tzplatform_config.h>
#include <unistd.h>

namespace bf = boost::filesystem;

namespace common_installer {

RequestMode GetRequestMode() {
  return (getuid() == tzplatform_getuid(TZ_SYS_GLOBALAPP_USER) ||
          getuid() == 0) ?
      RequestMode::GLOBAL : RequestMode::USER;
}

// Now, preload app is always installed RO location.
const char *GetRootAppPath(bool is_preload) {
  return GetRequestMode() == RequestMode::USER ?
      tzplatform_getenv(TZ_USER_APP) : is_preload ?
      tzplatform_getenv(TZ_SYS_RO_APP) : tzplatform_getenv(TZ_SYS_RW_APP);
}

boost::filesystem::path GetZipPackageLocation(const std::string& pkgid,
                                              bool is_preload) {
  bf::path zip_location = GetRequestMode() == RequestMode::USER ?
        tzplatform_getenv(TZ_USER_APPROOT) : is_preload ?
        tzplatform_getenv(TZ_SYS_RO_DESKTOP_APP) :
            tzplatform_getenv(TZ_SYS_RW_DESKTOP_APP);
  zip_location /= ".image";
  zip_location /= pkgid;
  return zip_location;
}

}  // namespace common_installer
