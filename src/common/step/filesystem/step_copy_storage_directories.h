// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_FILESYSTEM_STEP_COPY_STORAGE_DIRECTORIES_H_
#define COMMON_STEP_FILESYSTEM_STEP_COPY_STORAGE_DIRECTORIES_H_

#include <boost/filesystem/path.hpp>

#include <manifest_parser/utils/logging.h>

#include "common/step/step.h"

namespace common_installer {
namespace filesystem {

/**
 * \brief step responsible for copying "data" and "shared" dirs
 *        to the proper location during update or deinstallation.
 *        Used by TPK and WGT
 */
class __attribute__ ((visibility ("default"))) StepCopyStorageDirectories : public common_installer::Step {
 public:
  using Step::Step;

  /**
   * \brief main logic of copying data into directories
   *
   * \return Status::OK if success, Status::ERROR otherwise
   */
  Status process() override;
  Status clean() override { return Status::OK; }

  /**
   * \brief restores original content of data and shared dirs
   *
   * \return Status::OK if success, Status::ERROR otherwise
   */
  Status undo() override;

  /**
   * \brief checks if needed paths/data are provided
   *
   * \return Status::OK if success, Status::ERROR otherwise
   */
  Status precheck() override;

 protected:
  bool MoveAppStorage(const boost::filesystem::path& in_src,
                      const boost::filesystem::path& in_dst,
                      const char *key, bool merge_dirs = false);
  bool CacheDir();
  boost::filesystem::path backup_path_;
  SCOPE_LOG_TAG(CopyStorageDirectories)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_FILESYSTEM_STEP_COPY_STORAGE_DIRECTORIES_H_
