// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_COPY_BACKUP_H_
#define COMMON_STEP_STEP_COPY_BACKUP_H_

#include <boost/filesystem/path.hpp>

#include <manifest_parser/utils/logging.h>

#include "common/installer_context.h"
#include "common/step/step.h"

namespace common_installer {
namespace backup {

/**
 * \brief Responsible for backuping original package resources during
 *        update or uninstallation and copying new content of the package.
 *        Used by WGT and TPK backend
 */
class StepCopyBackup : public Step {
 public:
  using Step::Step;

  /**
   * \brief main logic of creating backup and copying new content
   *
   * \return Status::OK
   */
  Status process() override;

  /**
   * \brief removing backup files
   *
   * \return Status::OK if success, Status::ERROR otherwise
   */
  Status clean() override;

  /**
   * \brief Restoring original pacakge files
   *
   * \return Status::OK if success, Status::Error otherwise
   */
  Status undo() override;

  /**
   * \brief checks if necessary paths are available
   *
   * \return Status::OK if success, Status::Error otherwise
   */
  Status precheck() override;

 private:
  bool CreateBackup();
  bool CreateNewContent();
  bool CleanBackupDirectory();
  bool RollbackApplicationDirectory();

  boost::filesystem::path install_path_;
  boost::filesystem::path backup_path_;

  SCOPE_LOG_TAG(CopyBackup)
};

}  // namespace backup
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_COPY_BACKUP_H_
