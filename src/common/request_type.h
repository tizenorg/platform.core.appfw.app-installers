// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_REQUEST_TYPE_H_
#define COMMON_REQUEST_TYPE_H_

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

}  // namespace common_installer

#endif  // COMMON_REQUEST_TYPE_H_

