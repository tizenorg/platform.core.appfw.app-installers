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

class PkgmgrSignal;
class PkgMgrInterface;
typedef std::shared_ptr<PkgMgrInterface> PkgMgrPtr;

/**
 * \brief The PkgmgrInstallerInterface class
 *        Interface defining strategy for creation of pkgmgr_installer object
 *        and PkgmgrSignal object.
 *
 * This interface is injected to PkgMgrInterface class to decide:
 *  - how to create pkgmgr_installer,
 *  - if to create PkgmgrSignal object,
 *  - what installation mode should be set in installer context.
 */
class PkgmgrInstallerInterface {
 public:
  virtual bool CreatePkgMgrInstaller(pkgmgr_installer** installer,
                             InstallationMode* mode) = 0;
  virtual bool ShouldCreateSignal() const = 0;
};

/**
 * \brief The PkgmgrInstaller class
 *        Implementation of PkgmgrInstallerInterface that handles creation of
 *        pkgmgr_installer class in online and offline mode.
 */
class PkgmgrInstaller : public PkgmgrInstallerInterface {
 public:
  bool CreatePkgMgrInstaller(pkgmgr_installer** installer,
                             InstallationMode* mode) override;
  bool ShouldCreateSignal() const override;
};

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
   * Returns uid passed from pkgmgr_installer
   *
   * \return uid retrieved from pkgmgr_installer
   */
  uid_t GetUid() const;

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
   * \param pkgmgr_installer_interface interface defining strategy of creation
   *                                   of pkgmgr_installer
   * \param interface pointer to AppQueryInterface
   *
   * \return Smart pointer to the PkgMgrInterface
   */
  static PkgMgrPtr Create(int argc, char** argv,
        PkgmgrInstallerInterface* pkgmgr_installer_interface,
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
  * Returns True if move request is to external. Otherwise, return false
  *
  * \return True if move request is to external. Otherwise, return false
  */
  bool GetIsMoveToExternal();

  /**
  * Returns True if the request is for preload. Otherwise, return false
  *
  * \return True if the request is for preload. Otherwise, return false
  */
  bool GetIsPreloadRequest();

  /**
  * Returns True if the request is for force-remove. Otherwise, return false
  *
  * \return True if the request is for force-remove. Otherwise, return false
  */
  bool GetIsForceRemoval();

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

  /**
   * @brief CreatePkgmgrSignal
   *
   * @return creates pkgmgr signal
   */
  std::unique_ptr<PkgmgrSignal> CreatePkgmgrSignal() const;

  /** PkgMgrInstance destructor. */
  ~PkgMgrInterface();

 private:
  explicit PkgMgrInterface(PkgmgrInstallerInterface* pkgmgr_installer_interface,
                           AppQueryInterface* interface)
      : pi_(nullptr),
        install_mode_(InstallationMode::ONLINE),
        is_app_installed_(false),
        pkgmgr_installer_interface_(pkgmgr_installer_interface),
        query_interface_(interface) {}
  int InitInternal(int argc, char** argv);

  pkgmgr_installer* pi_;
  InstallationMode install_mode_;
  bool is_app_installed_;
  PkgmgrInstallerInterface* pkgmgr_installer_interface_;

  AppQueryInterface* query_interface_;

  SCOPE_LOG_TAG(PkgMgrInterface)
  DISALLOW_COPY_AND_ASSIGN(PkgMgrInterface);
};

}  // namespace common_installer

#endif  // COMMON_PKGMGR_INTERFACE_H_
