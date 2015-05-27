/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_copy.h"

#include <cassert>
#include <cstring>
#include <string>

#include "utils/file_util.h"

namespace common_installer {
namespace copy {

namespace bf = boost::filesystem;
namespace bs = boost::system;

Step::Status StepCopy::precheck() {
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

  if (context_->unpacked_dir_path.get().empty()) {
    LOG(ERROR) << "unpacked_dir_path attribute is empty";
    return Step::Status::INVALID_VALUE;
  }
  if (!boost::filesystem::exists(context_->unpacked_dir_path.get())) {
    LOG(ERROR) << "unpacked_dir_path ("
               << context_->unpacked_dir_path.get()
               << ") path does not exist";
    return Step::Status::INVALID_VALUE;
  }

  if (!context_->manifest_data.get()) {
    LOG(ERROR) << "manifest_data attribute is empty";
    return Step::Status::INVALID_VALUE;
  }

  if (context_->pkgid.get().empty()) {
    LOG(ERROR) << "pkgid attribute is empty";
    return Step::Status::INVALID_VALUE;
  }

  // TODO(p.sikorski) asserts?

  return Step::Status::OK;
}

Step::Status StepCopy::process() {
  // set application path
  context_->application_path.set(
    context_->root_application_path.get() / context_->pkgid.get());

  bf::path install_path = bf::path(context_->application_path.get());

  context_->pkg_path.set(install_path.string());

  // FIXME: correctly order app's data.
  // If there is 1 app in package, the app's data are stored in
  // <pkg_path>/<app_id>/
  // If there are >1 apps in the package, app's data are stored in <pkg_path>
  // considering that multiple apps data are already separated in folders.
  manifest_x *m = context_->manifest_data.get();
  if ((m->uiapplication && !m->uiapplication->next && !m->serviceapplication) ||
      (m->serviceapplication && !m->serviceapplication->next &&
      !m->uiapplication)) {
    install_path /= bf::path(context_->manifest_data.get()->mainapp_id);
  }

  bs::error_code error;
  bf::create_directories(install_path.parent_path(), error);
  if (error) {
    LOG(ERROR) << "Cannot create directory: "
               << install_path.parent_path().string();
    return Step::Status::ERROR;
  }
  if (!utils::MoveDir(context_->unpacked_dir_path.get(), install_path)) {
    LOG(ERROR) << "Cannot move widget directory to install path";
    return Status::ERROR;
  }

  LOG(INFO) << "Successfully move: " << context_->unpacked_dir_path.get()
            << " to: " << install_path << " directory";
  return Status::OK;
}

Step::Status StepCopy::clean() {
  return Status::OK;
}

Step::Status StepCopy::undo() {
  if (bf::exists(context_->pkg_path.get()))
    bf::remove_all(context_->pkg_path.get());
  return Status::OK;
}

}  // namespace copy
}  // namespace common_installer
