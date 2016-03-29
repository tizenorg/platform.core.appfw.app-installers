// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_CONFIGURATION_STEP_CONFIGURE_H_
#define COMMON_STEP_CONFIGURATION_STEP_CONFIGURE_H_

#include <manifest_parser/utils/logging.h>

#include "common/installer_context.h"

#include "common/pkgmgr_interface.h"
#include "common/step/step.h"

namespace common_installer {
namespace configuration {

/**
 * \brief Installation,Update, Deinstallation, RDS.
 *        Responsible for filling InstallerContext based on the request type.
 *
 * Based on started request, process fills InstallerContext with proper data.
 */
class StepConfigure : public Step {
 public:
  /**
   * \brief Constuctor
   *
   * \param context pointer to InstallerContext structure
   * \param pkgmgr pointer to Pkgmgr interface
   */
  __attribute__ ((visibility ("default"))) StepConfigure(InstallerContext* context, PkgMgrPtr pkgmgr);

  /**
   * \brief configuration based on pacakge typ, request mode, user
   *
   * \return Status::OK if success, Status::ERROR otherwise
   */
  __attribute__ ((visibility ("default"))) Status process() override;

  /**
   * \brief Recovery file reset
   *
   * \return Status::OK
   */
  __attribute__ ((visibility ("default"))) Status clean() override;

  __attribute__ ((visibility ("default"))) Status undo() override { return Status::OK; }

  /**
   * \brief checks, if backend not launched with root uid
   *
   * \return Status::OK if normal user, Status::ERROR if root
   */
  __attribute__ ((visibility ("default"))) Status precheck() override;

 private:
  __attribute__ ((visibility ("default"))) bool SetupRootAppDirectory();
  __attribute__ ((visibility ("default"))) void SetupRequestMode();
  __attribute__ ((visibility ("default"))) void SetupRequestType();
  __attribute__ ((visibility ("default"))) void SetupFileCreationMask();
  __attribute__ ((visibility ("default"))) void SetupIsPreloadRequest();
  __attribute__ ((visibility ("default"))) void SetupIsForceRemoval();

  PkgMgrPtr pkgmgr_;

  SCOPE_LOG_TAG(Configure)
};

}  // namespace configuration
}  // namespace common_installer

#endif  // COMMON_STEP_CONFIGURATION_STEP_CONFIGURE_H_
