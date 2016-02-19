// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_BACKUP_ICONS_H_
#define COMMON_STEP_STEP_BACKUP_ICONS_H_

#include <boost/filesystem/path.hpp>
#include <manifest_parser/utils/logging.h>
#include <utility>
#include <vector>

#include "common/installer_context.h"
#include "common/step/step.h"

namespace common_installer {
namespace backup {

/**
 *\brief Step responsible for backuping icons during update and uninstallation.
 *       Used by TPK and WGT backend
 */
class StepBackupIcons : public Step {
 public:
  using Step::Step;

  /**
   * \brief main logic of backuping icons
   *
   * \return Status::OK, if successful backup, Status::ERROR otherwise
   */
  Status process() override;

  /**
   * \brief removes backup files after successful update/deinstallation
   *
   * \return Status::OK if success, Status::ERROR otherwise
   */
  Status clean() override;

  /**
   * \brief restores backup icons.
   *
   * \return Status:OK if success, Status::ERROR othewise
   */
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
