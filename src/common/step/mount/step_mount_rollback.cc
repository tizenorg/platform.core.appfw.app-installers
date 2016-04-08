// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/mount/step_mount_rollback.h"

#include <boost/filesystem/path.hpp>

#include <string>

#include "common/backup_paths.h"
#include "common/request.h"
#include "common/tzip_interface.h"

namespace bf = boost::filesystem;

namespace common_installer {
namespace mount {

Step::Status StepMountRollback::undo() {
  bf::path mount_point = GetMountLocation(context_->pkg_path.get());
  bf::path zip_file = GetZipPackageLocation(context_->pkgid.get(),
                                            context_->is_preload_request.get());
  TzipInterface tzip(mount_point.string());
  if (!tzip.MountZip(zip_file.string())) {
    LOG(ERROR) << "Failed to mount: " << zip_file << " at: " << mount_point;
    return Status::APP_DIR_ERROR;
  }
  return Status::OK;
}


}  // namespace mount
}  // namespace common_installer
