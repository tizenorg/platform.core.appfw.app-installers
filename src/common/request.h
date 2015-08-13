// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_REQUEST_H_
#define COMMON_REQUEST_H_

namespace common_installer {

/** Request type received from pkgmgr_installer */
enum class RequestType : int {
  Unknown = 0,
  Install,
  Update,
  Uninstall,
  Reinstall,
  Recovery
};

/** Request mode (USER vs GLOBAL) */
enum class RequestMode : int {
  USER,
  GLOBAL
};

/** Get mode for current request (GLOBAL/USER) */
RequestMode GetRequestMode();

/** Get apps root path for current request (GLOBAL/USER) */
const char *GetRootAppPath();

}  // namespace common_installer

#endif  // COMMON_REQUEST_H_

