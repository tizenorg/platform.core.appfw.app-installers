// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_PKGMGR_INTERFACE_H_
#define COMMON_PKGMGR_INTERFACE_H_

#include <pkgmgr_installer.h>

#include <memory>

#include "common/utils/macros.h"
#include "common/utils/logging.h"

namespace common_installer {

class PkgMgrInterface;
typedef std::shared_ptr<PkgMgrInterface> PkgMgrPtr;

/** Class that covers pkgmgr_installer basic platform calls.
 *
 *  PkgMgr covers all pkgmgr_installer platform calls (and manages its
 *  creation/destruction.
 */
class PkgMgrInterface {
 public:
  /** Request type received from pkgmgr_installer
   */
  enum class Type {
    Unknown,
    Install,
    Update,
    Uninstall,
    Reinstall
  };

  /** Returns Request type passed from pkgmgr_installer
   */
  PkgMgrInterface::Type GetRequestType() const;

  /** Returns Request info passed from pkgmgr_installer
   */
  const char *GetRequestInfo() const;

  /** Returns instance of PkgrMgr (Singleton pattern).
   *
   *  However, Init method has to be called first (otherwise, this Instance
   *  returns nullptr).
   *
   *  @see PkgMgr::Init(int argc, char** argv)
   */
  static PkgMgrPtr Instance();

  /** Initialize PkgMgrInterface.
   */
  static int Init(int argc, char** argv);

  /** Get Raw pointer to pkgmgr_installer object
   *
   *  It should not be used (PkgMgrInterface can destroy it
   */
  DEPRECATED pkgmgr_installer *GetRawPi() const { return pi_; }

  /** PkgMgrInstance destructor.
   *
   */
  ~PkgMgrInterface();

 private:
  PkgMgrInterface() :pi_(nullptr) {}
  int InitInternal(int argc, char** argv);

  pkgmgr_installer* pi_;
  static PkgMgrPtr instance_;

  SCOPE_LOG_TAG(PkgMgrInterface)
  DISALLOW_COPY_AND_ASSIGN(PkgMgrInterface);
};

}  // namespace common_installer

#endif  // COMMON_PKGMGR_INTERFACE_H_
