/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */

#ifndef TPK_TPK_INSTALLER_H_
#define TPK_TPK_INSTALLER_H_

#include "common/app_installer.h"
#include "common/pkgmgr_interface.h"
#include "common/utils/logging.h"

namespace tpk {

/**
 * @brief The TpkInstaller class
 *        Handles request of tpk packages.
 *
 * This class is main class for installation/update/deinstallation of tpk
 * packages. Pkgmgr request is parsed within and sequence of steps is built to
 * be run.
 */
class TpkInstaller : public common_installer::AppInstaller {
 public:
  TpkInstaller(common_installer::PkgMgrPtr pkgmgr);
  ~TpkInstaller();
  void Prepare();

 private:
  void InstallSteps();
  void UpdateSteps();
  void UninstallSteps();
  void ReinstallSteps();
  void RecoverySteps();

  SCOPE_LOG_TAG(TpkInstaller)
};

}  // namespace tpk

#endif  // TPK_TPK_INSTALLER_H_
