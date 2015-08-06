// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_PKGMGR_INTERFACE_H_
#define COMMON_PKGMGR_INTERFACE_H_

#include <pkgmgr_installer.h>

#include <memory>

#include "common/app_query_interface.h"
#include "common/request_type.h"
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
  /** Returns Request type passed from pkgmgr_installer
   */
  RequestType GetRequestType() const;

  /** Returns Request info passed from pkgmgr_installer
   */
  const char *GetRequestInfo() const;

  /** Creates PkgMgrInterface.
   */
  static PkgMgrPtr Create(int argc, char** argv,
        AppQueryInterface* interface = nullptr);

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
  explicit PkgMgrInterface(AppQueryInterface* interface)
      : pi_(nullptr),
        query_interface_(interface) {}
  int InitInternal(int argc, char** argv);

  pkgmgr_installer* pi_;
  bool is_app_installed_;
  AppQueryInterface* query_interface_;

  SCOPE_LOG_TAG(PkgMgrInterface)
  DISALLOW_COPY_AND_ASSIGN(PkgMgrInterface);
};

}  // namespace common_installer

#endif  // COMMON_PKGMGR_INTERFACE_H_
