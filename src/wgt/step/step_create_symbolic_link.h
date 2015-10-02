// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_STEP_CREATE_SYMBOLIC_LINK_H_
#define WGT_STEP_STEP_CREATE_SYMBOLIC_LINK_H_

#include <boost/filesystem.hpp>

#include "common/app_installer.h"
#include "common/installer_context.h"
#include "common/step/step.h"
#include "common/utils/logging.h"

namespace wgt {
namespace filesystem {

class StepCreateSymbolicLink : public common_installer::Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override;
  Status precheck() override { return Status::OK; }

  SCOPE_LOG_TAG(SymbolicLink)
};

}  // namespace filesystem
}  // namespace wgt

#endif  // WGT_STEP_STEP_CREATE_SYMBOLIC_LINK_H_
