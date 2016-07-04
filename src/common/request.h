// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_REQUEST_H_
#define COMMON_REQUEST_H_

#include <boost/filesystem/path.hpp>

#include <string>

namespace common_installer {

/** Request type received from pkgmgr_installer */
enum class RequestType : int {
  Unknown = 0,
  Install,
  Update,
  Uninstall,
  Reinstall,
  Clear,
  Delta,
  Move,
  Recovery,
  MountInstall,
  MountUpdate,
  ManifestDirectInstall,
  ManifestDirectUpdate,
  DisablePkg,
  EnablePkg
};

/** Request mode (USER vs GLOBAL) */
enum class RequestMode : int {
  USER,
  GLOBAL
};

/**
 * Get mode for current request (GLOBAL/USER)
 *
 * \return request mode
 */
RequestMode GetRequestMode(uid_t uid);

/**
 * Get apps root path for current request (GLOBAL/USER)
 *
 * \return root application path (eg. $HOME/apps_rw/)
 */
const char* GetRootAppPath(bool is_preload, uid_t uid);

}  // namespace common_installer

#endif  // COMMON_REQUEST_H_

