// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_COPY_BACKUP_H_
#define COMMON_STEP_STEP_COPY_BACKUP_H_

#include <boost/filesystem/path.hpp>

#include "common/context_installer.h"
#include "common/step/step.h"
#include "utils/logging.h"

namespace common_installer {
namespace copy_backup {

class StepCopyBackup : public Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override;
  Status undo() override;
  Status precheck() override { return Status::OK; }

 private:
  bool CleanBackupDirectory();
  bool RollbackApplicationDirectory();

  boost::filesystem::path install_path_;
  boost::filesystem::path backup_path_;

  SCOPE_LOG_TAG(CopyBackup)
};

}  // namespace copy_backup
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_COPY_BACKUP_H_
