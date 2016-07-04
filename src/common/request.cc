// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/request.h"
#include <tzplatform_config.h>
#include <unistd.h>

namespace bf = boost::filesystem;

namespace common_installer {

RequestMode GetRequestMode(uid_t uid) {
  return (uid == tzplatform_getuid(TZ_SYS_GLOBALAPP_USER) ||
          getuid() == 0) ?
      RequestMode::GLOBAL : RequestMode::USER;
}

// Now, preload app is always installed RO location.
const char* GetRootAppPath(bool is_preload, uid_t uid) {
  if (GetRequestMode(uid) == RequestMode::GLOBAL) {
    return is_preload ?
        tzplatform_getenv(TZ_SYS_RO_APP) : tzplatform_getenv(TZ_SYS_RW_APP);
  } else {
    tzplatform_set_user(uid);
    const char* rootpath = tzplatform_getenv(TZ_USER_APP);
    tzplatform_reset_user();
    return rootpath;
  }
}
}  // namespace common_installer
