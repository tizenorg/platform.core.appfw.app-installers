// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_PKGMGR_INTERFACE_H_
#define COMMON_PKGMGR_INTERFACE_H_

#include <boost/filesystem/path.hpp>
#include <manifest_parser/utils/logging.h>
#include <pkgmgr_installer.h>
#include <memory>

#include "common/app_query_interface.h"
#include "common/request.h"
#include "common/utils/macros.h"

namespace common_installer {

enum class InstallationMode {
  ONLINE,
  OFFLINE
};

class PkgMgrInterface;
typedef std::shared_ptr<PkgMgrInterface> PkgMgrPtr;

/**
 * \brief Encapsulates pkgmgr API which handles parsing backend options
 *        and returns values/modes for installation process.
 */
class PkgMgrInterface {
 public:
  /**
   * Returns Request type passed from pkgmgr_installer
   *
   * \return request type retrieved from pkgmgr_installer
   */
  RequestType GetRequestType() const;

  /**
   * Returns Request info passed from pkgmgr_installer
   *
   * \return request info retrieved from pkgmgr_installer
   */
  const char *GetRequestInfo() const;

  /**
   * Creates PkgMgrInterface
   *
   * \param argc main() argc argument passed to the backend
   * \param argv main() argv argument passed to the backend
   * \param interface pointer to AppQueryInterface
   *
   * \return Smart pointer to the PkgMgrInterface
   */
  static PkgMgrPtr Create(int argc, char** argv,
        AppQueryInterface* interface = nullptr);

  /**
  * Returns TEP path passed from pkgmgr_installer
  *
  * \return TEP path retrieved from pkgmgr_installer
  */
  boost::filesystem::path GetTepPath() const;

  /**
  * Returns True if TEP file should be moved. Otherwise, return false
  *
  * \return True if TEP file should be moved. Otherwise, return false
  */
  bool GetIsTepMove();

  /**
  * Returns True if the request is for preload. Otherwise, return false
  *
  * \return True if the request is for preload. Otherwise, return false
  */
  bool GetIsPreloadRequest();

  /**
   * Get Raw pointer to pkgmgr_installer object
   * NOTE: It should not be used (PkgMgrInterface can destroy it
   *
   * \return raw pkgmgr_installer pointer
   */
  DEPRECATED pkgmgr_installer *GetRawPi() const { return pi_; }

  /**
  * Returns installation mode
  *
  * \return 'ONLINE' for online installation, 'OFFLINE' otherwise
  */
  InstallationMode GetInstallationMode() const { return install_mode_; }


  /** PkgMgrInstance destructor. */
  ~PkgMgrInterface();

 private:
  explicit PkgMgrInterface(AppQueryInterface* interface)
      : pi_(nullptr),
        install_mode_(InstallationMode::ONLINE),
        is_app_installed_(false),
        query_interface_(interface) {}
  int InitInternal(int argc, char** argv);

  pkgmgr_installer* pi_;
  InstallationMode install_mode_;
  bool is_app_installed_;

  AppQueryInterface* query_interface_;

  SCOPE_LOG_TAG(PkgMgrInterface)
  DISALLOW_COPY_AND_ASSIGN(PkgMgrInterface);
};

}  // namespace common_installer

#endif  // COMMON_PKGMGR_INTERFACE_H_
