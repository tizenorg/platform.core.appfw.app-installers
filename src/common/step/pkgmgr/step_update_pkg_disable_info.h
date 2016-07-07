// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_PKGMGR_STEP_UPDATE_PKG_DISABLE_INFO_H_
#define COMMON_STEP_PKGMGR_STEP_UPDATE_PKG_DISABLE_INFO_H_

#include <common/app_installer.h>
#include <common/installer_context.h>
#include <common/step/step.h>

namespace common_installer {
namespace pkgmgr {

class StepUpdatePkgDisableInfo : public common_installer::Step {
 public:
  using Step::Step;
  enum class ActionType { Disable, Enable };

  explicit StepUpdatePkgDisableInfo(common_installer::InstallerContext* context,
                               ActionType action_type);

  Step::Status process() override;
  Step::Status clean() override { return Status::OK; }
  Step::Status undo() override;
  Status precheck() override;

 private:
  ActionType action_type_;
  STEP_NAME(UpdatePkgDisableInfo)
};

}  // namespace pkgmgr
}  // namespace common_installer

#endif  //COMMON_STEP_PKGMGR_STEP_UPDATE_PKG_DISABLE_INFO_H_
