// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_PKGMGR_STEP_CHECK_BLACKLIST_H_
#define COMMON_STEP_PKGMGR_STEP_CHECK_BLACKLIST_H_

#include "common/step/step.h"

namespace common_installer {
namespace pkgmgr {

class StepCheckBlacklist : public Step {
 public:
  using Step::Step;

  __attribute__ ((visibility ("default"))) Status process() override;
  __attribute__ ((visibility ("default"))) Status undo() override { return Status::OK; }
  __attribute__ ((visibility ("default"))) Status clean() override { return Status::OK; }
  __attribute__ ((visibility ("default"))) Status precheck() override;

  SCOPE_LOG_TAG(Blacklist)
};

}  // namespace pkgmgr
}  // namespace common_installer

#endif  // COMMON_STEP_PKGMGR_STEP_CHECK_BLACKLIST_H_
