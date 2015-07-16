// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_COPY_STORAGE_DIRECTORIES_H_
#define COMMON_STEP_STEP_COPY_STORAGE_DIRECTORIES_H_

#include <boost/filesystem/path.hpp>

#include "common/step/step.h"
#include "utils/logging.h"

namespace common_installer {
namespace filesystem {

class StepCopyStorageDirectories : public common_installer::Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override;
  Status precheck() override;

 protected:
  bool MoveAppStorage(const boost::filesystem::path& in_src,
                      const boost::filesystem::path& in_dst,
                      const char *key);

  boost::filesystem::path backup_path_;

  SCOPE_LOG_TAG(CreateStorageDirectories)
};

}  // namespace filesystem
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_COPY_STORAGE_DIRECTORIES_H_
