// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_BACKUP_ICONS_H_
#define COMMON_STEP_STEP_BACKUP_ICONS_H_

#include <boost/filesystem/path.hpp>
#include <utility>
#include <vector>

#include "common/context_installer.h"
#include "common/step/step.h"
#include "common/utils/logging.h"

namespace common_installer {
namespace backup {

class StepBackupIcons : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override;
  Status undo() override;
  Status precheck() override { return Status::OK; }

 private:
  void RemoveBackupIcons();

  std::vector<std::pair<boost::filesystem::path, boost::filesystem::path>>
      icons_;

  SCOPE_LOG_TAG(BackupIcons)
};

}  // namespace backup
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_BACKUP_ICONS_H_
