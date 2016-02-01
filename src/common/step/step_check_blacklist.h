// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_CHECK_BLACKLIST_H_
#define COMMON_STEP_STEP_CHECK_BLACKLIST_H_

#include "common/step/step.h"

namespace common_installer {
namespace blacklist {

class StepCheckBlacklist : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status undo() override { return Status::OK; }
  Status clean() override { return Status::OK; }
  Status precheck() override;

  SCOPE_LOG_TAG(Blacklist)
};

}  // namespace blacklist
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_CHECK_BLACKLIST_H_
