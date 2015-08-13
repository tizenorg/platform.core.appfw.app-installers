// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/request.h"
#include <tzplatform_config.h>
#include <unistd.h>

namespace common_installer {

RequestMode GetRequestMode() {
  return (getuid() == tzplatform_getuid(TZ_SYS_GLOBALAPP_USER)) ?
      RequestMode::GLOBAL : RequestMode::USER;
}

const char *GetRootAppPath() {
  return GetRequestMode() == RequestMode::USER ?
      tzplatform_getenv(TZ_USER_APP) : tzplatform_getenv(TZ_SYS_RW_APP);
}

}  // namespace common_installer
