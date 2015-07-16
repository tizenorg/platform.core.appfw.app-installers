// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_wgt_create_storage_directories.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>

#include "common/utils/file_util.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace {

const char kSharedLocation[] = "shared";
const char kResWgtSubPath[] = "res/wgt";

}  // namespace

namespace wgt {
namespace filesystem {

common_installer::Step::Status StepWgtCreateStorageDirectories::process() {
  if (!PrivateDir())
    return Status::ERROR;

  char rel_version =
      context_->config_data.get().required_tizen_version.get().at(0);

  if ((rel_version-'0') < 3) {
    LOG(DEBUG) << "Shared directory preparation for tizen 2.x";
    if (!ShareDir())
      return Status::ERROR;
  } else {
    LOG(DEBUG) << "Shared directory preparation for tizen 3.x";
    if (!ShareDirFor3x())
      return Status::ERROR;
  }

  if (!SubShareDir())
    return Status::ERROR;

  return Status::OK;
}

bool StepWgtCreateStorageDirectories::ShareDirFor2x() {
  bs::error_code error_code;
  bf::path shared_path = context_->pkg_path.get() / kSharedLocation;
  bf::create_directory(shared_path, error_code);
  if (error_code) {
    LOG(ERROR) << "Failed to create shared directory for widget";
    return false;
  }
  return true;
}

bool StepWgtCreateStorageDirectories::ShareDirFor3x() {
  bf::path res_wgt_path = context_->pkg_path.get() / kResWgtSubPath;
  if (!bf::exists(res_wgt_path / kSharedLocation)) {
    if (!ShareDir())
      return false;
  } else {
    bf::path src = res_wgt_path / kSharedLocation;
    bf::path dst = context_->pkg_path.get() / kSharedLocation;
    if (!common_installer::MoveDir(src, dst)) {
      LOG(ERROR) << "Failed to move shared data from res/wgt to shared";
      return false;
    }

    bs::error_code error_code;
    bf::create_symlink(dst, src, error_code);
    if (error_code) {
      LOG(ERROR) << "Failed to create symbolic link for shared dir"
        << boost::system::system_error(error_code).what();
      return false;
    }
  }  // else
  return true;
}

}  // namespace filesystem
}  // namespace wgt
