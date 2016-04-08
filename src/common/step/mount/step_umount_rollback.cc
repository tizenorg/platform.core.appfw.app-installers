// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/mount/step_umount_rollback.h"

#include <boost/filesystem/path.hpp>

#include <string>

#include "common/backup_paths.h"
#include "common/tzip_interface.h"

namespace bf = boost::filesystem;

namespace common_installer {
namespace mount {

Step::Status StepUmountRollback::undo() {
  bf::path mount_point = GetMountLocation(context_->pkg_path.get());
  TzipInterface tzip(mount_point.string());
  if (!tzip.UnmountZip()) {
    LOG(ERROR) << "Failed to umount: " << mount_point;
    return Status::APP_DIR_ERROR;
  }
  return Status::OK;
}

}  // namespace mount
}  // namespace common_installer
