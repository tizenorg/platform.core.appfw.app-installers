// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_remove_files.h"

#include <boost/filesystem/operations.hpp>
#include <boost/system/error_code.hpp>

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
  return Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer
