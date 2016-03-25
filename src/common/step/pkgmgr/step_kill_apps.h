// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_PKGMGR_STEP_KILL_APPS_H_
#define COMMON_STEP_PKGMGR_STEP_KILL_APPS_H_

#include <manifest_parser/utils/logging.h>

#include "common/installer_context.h"
#include "common/step/step.h"

namespace common_installer {
namespace pkgmgr {

/**
 * \brief Step responsible for killing applications of the package that
 *        is being updated, uninstalled
 *
 */
class StepKillApps : public Step {
 public:
  using Step::Step;

  enum class PackageStatus {
    NORMAL, // Package will be updated, uninstalled, delta
    DISABLE //Package will be disabled
  };

  explicit StepKillApps(common_installer::InstallerContext *context,
      PackageStatus package_status);

  /**
   * \brief main logic for killing applications
   *
   * \return Status::OK
   */
  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }

  /**
   * \brief checks if neccessary data is provided
   *
   * \return Status::OK if success, Status::ERROR otherwise
   */
  Status precheck() override;

  SCOPE_LOG_TAG(KillApps)

 private:
  PackageStatus package_status_;

};

}  // namespace pkgmgr
}  // namespace common_installer

#endif  // COMMON_STEP_PKGMGR_STEP_KILL_APPS_H_
