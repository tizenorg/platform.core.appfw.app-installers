// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_REMOVE_ICONS_H_
#define COMMON_STEP_STEP_REMOVE_ICONS_H_

#include <boost/filesystem/path.hpp>

#include <utility>
#include <vector>

#include "common/context_installer.h"
#include "common/step/step.h"
#include "common/utils/logging.h"

namespace common_installer {
namespace filesystem {

class StepRemoveIcons : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override;
  Status undo() override;
  Status precheck() override;

 private:
  std::vector<std::pair<boost::filesystem::path, boost::filesystem::path>>
      backups_;

  SCOPE_LOG_TAG(RemoveIcons)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_REMOVE_ICONS_H_
