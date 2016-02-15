// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_remove_files.h"

#include <boost/filesystem/operations.hpp>
#include <boost/system/error_code.hpp>

#include <string>

#include "common/utils/subprocess.h"

namespace bs = boost::system;
namespace bf = boost::filesystem;


namespace common_installer {
namespace filesystem {

Step::Status StepRemoveFiles::precheck() {
  if (!context_->manifest_data.get()) {
    LOG(ERROR) << "manifest_data attribute is empty";
    return Step::Status::MANIFEST_NOT_FOUND;
  }

  // Even though, the below checks can fail, StepRemoveFiles should still try
  // to remove the files
  if (context_->pkg_path.get().empty())
    LOG(ERROR) << "pkg_path attribute is empty";
  else if (!bf::exists(context_->pkg_path.get()))
    LOG(ERROR) << "pkg_path ("
               << context_->pkg_path.get()
               << ") path does not exist";
  // TODO(p.sikorski) check context_->uid.get()

  return Step::Status::OK;
}

Step::Status StepRemoveFiles::process() {
  bs::error_code error;
  bf::path pkg_path(context_->pkg_path.get());
  bf::remove_all(pkg_path, error);

  if (error) {
    LOG(ERROR) << "Can't remove directory:" << context_->pkg_path.get().c_str();
  } else {
    LOG(DEBUG) << "Removed directory: " << context_->pkg_path.get();
  }

  if (context_->request_mode.get() == RequestMode::GLOBAL) {
    RemoveSharedDirs();
  }

  return Status::OK;
}

bool StepRemoveFiles::RemoveSharedDirs() {
  std::string package_id = context_->pkgid.get();
  Subprocess pkgdir_tool_process("/usr/bin/pkgdir-tool");
  pkgdir_tool_process.RunWithArgs({"--delete", package_id});
  int result = pkgdir_tool_process.Wait();
  if (result) {
    LOG(ERROR) << "Failed to delete shared dirs for users";
    return false;
  }
  return true;
}

}  // namespace filesystem
}  // namespace common_installer
