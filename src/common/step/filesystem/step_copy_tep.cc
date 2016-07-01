/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_copy_tep.h"

#include <cassert>
#include <cstring>
#include <string>

#include "common/paths.h"
#include "common/utils/file_util.h"

namespace common_installer {
namespace filesystem {

namespace bf = boost::filesystem;
namespace bs = boost::system;

Step::Status StepCopyTep::precheck() {
  if (context_->root_application_path.get().empty()) {
    LOG(ERROR) << "root_application_path attribute is empty";
    return Step::Status::INVALID_VALUE;
  }

  if (!boost::filesystem::exists(context_->root_application_path.get())) {
    LOG(ERROR) << "root_application_path ("
               << context_->root_application_path.get()
               << ") path does not exist";
    return Step::Status::INVALID_VALUE;
  }

  if (context_->pkgid.get().empty()) {
    LOG(ERROR) << "pkgid attribute is empty";
    return Step::Status::INVALID_VALUE;
  }

  return Step::Status::OK;
}

Step::Status StepCopyTep::process() {
  if (context_->tep_path.get().empty())
    return Step::Status::OK;

  context_->pkg_path.set(
    context_->root_application_path.get() / context_->pkgid.get());

  bf::path tep_path;
  if (context_->external_storage) {
    tep_path = GetExternalTepPath(context_->request_mode.get(),
                                  context_->uid.get());
  } else {
    tep_path = GetInternalTepPath(context_->pkg_path.get());
  }

  // Keep filename of app store supplied file. Filename contains hash that
  // appstore uses to identify version of tep on device.
  tep_path /= context_->tep_path.get().filename();

  if (!bf::exists(tep_path.parent_path())) {
    bs::error_code error;
    bf::create_directories(tep_path.parent_path(), error);
    if (error) {
      LOG(ERROR) << "Cannot create tep parent directory";
      return Status::APP_DIR_ERROR;
    }
  }

  if (context_->is_tep_move.get()) {
    if (!MoveFile(context_->tep_path.get(), tep_path)) {
      LOG(ERROR) << "Cannnot move TEP file into install path";
      return Step::Status::APP_DIR_ERROR;
    }
  } else {
    if (!CopyFile(context_->tep_path.get(), tep_path)) {
      LOG(ERROR) << "Cannot copy TEP file [" << context_->tep_path.get() <<
          "] into install path [" << tep_path << "]";
      return Step::Status::APP_DIR_ERROR;
    }
  }
  context_->tep_path.set(tep_path);
  context_->manifest_data.get()->tep_name =
      strdup(context_->tep_path.get().c_str());

  return Step::Status::OK;
}

Step::Status StepCopyTep::undo() {
  bs::error_code error;
  if (bf::exists(context_->tep_path.get())) {
    bf::remove_all(context_->tep_path.get(), error);
  }

  // remove tep file is installed outside package path
  if (context_->external_storage) {
    bf::path tep_path = GetExternalTepPath(context_->request_mode.get(),
                                           context_->uid.get());
    tep_path /= context_->tep_path.get().filename();
    if (bf::exists(tep_path)) {
      bf::remove_all(tep_path, error);
    }
  }
  return Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer
