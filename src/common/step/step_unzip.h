/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_UNZIP_H_
#define COMMON_STEP_STEP_UNZIP_H_

#include <boost/filesystem/path.hpp>

#include "common/context_installer.h"
#include "common/step/step.h"
#include "common/utils/logging.h"

namespace common_installer {
namespace unzip {

class StepUnzip : public Step {
 public:
  using Step::Step;
  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override;
  Status precheck() override;

  SCOPE_LOG_TAG(Unzip)
};

}  // namespace unzip
}  // namespace common_installer


#endif  // COMMON_STEP_STEP_UNZIP_H_
