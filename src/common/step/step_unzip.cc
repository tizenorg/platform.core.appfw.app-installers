/* 2014, Copyright © Intel Coporation, APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_unzip.h"

#include <tzplatform_config.h>

#include <boost/filesystem.hpp>
#include <boost/chrono/detail/system.hpp>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <climits>
#include <cstring>
#include <string>

#include "utils/file_util.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace {

bool CheckFreeSpaceAtPath(int64_t required_size,
    const boost::filesystem::path& target_location) {
  bs::error_code error;
  boost::filesystem::path root = target_location;
  while (!bf::exists(root) && root != root.root_path()) {
    root = root.parent_path();
  }
  if (!bf::exists(root)) {
    LOG(ERROR) << "No mount point for path: " << target_location;
    return false;
  }
  bf::space_info space_info = bf::space(root, error);
  if (error) {
    LOG(ERROR) << "Failed to get space_info: " << error.message();
    return false;
  }

  return (space_info.free >= static_cast<uint64_t>(required_size));
}

}  // namespace

namespace common_installer {
namespace unzip {

StepUnzip::StepUnzip(ContextInstaller* context)
    : Step(context) {}

Step::Status StepUnzip::precheck() {
  if (context_->file_path.get().empty()) {
    LOG(ERROR) << "file_path attribute is empty";
    return Step::Status::INVALID_VALUE;
  }
  if (!boost::filesystem::exists(context_->file_path.get())) {
    LOG(ERROR) << "file_path ("
               << context_->file_path.get()
               << ") path does not exist";
    return Step::Status::INVALID_VALUE;
  }

  if (context_->root_application_path.get().empty()) {
    LOG(ERROR) << "root_application_path attribute is empty";
    return Step::Status::INVALID_VALUE;
  }
  if (!boost::filesystem::exists(context_->root_application_path.get())) {
    LOG(ERROR) << "root_application_path ("
               << context_->root_application_path.get()
               << ") path does not exist";
    // TODO(p.sikorski) maybe it should be created (instead of returning error)
    // but, if so, then it should be created in a separate step, eg.
    // AppInstallerConfigure or something similar
    return Step::Status::INVALID_VALUE;
  }

  return Step::Status::OK;
}

Step::Status StepUnzip::process() {
  bf::path tmp_dir =
      utils::GenerateTmpDir(context_->root_application_path.get());

  if (!utils::CreateDir(tmp_dir)) {
    LOG(ERROR) << "Failed to create temp directory: " << tmp_dir;
    return Step::Status::ERROR;
  }

  int64_t required_size =
      utils::GetUnpackedPackageSize(context_->file_path.get());

  if (required_size == -1) {
    LOG(ERROR) << "Couldn't get uncompressed size for package: "
               << context_->file_path.get();
    return Step::Status::ERROR;
  }

  LOG(DEBUG) << "Required size for application: " << required_size << "B";

  if (!CheckFreeSpaceAtPath(required_size, tmp_dir)) {
    LOG(ERROR) << "There is not enough space to unpack application files";
    return Step::Status::OUT_OF_SPACE;
  }

  if (!CheckFreeSpaceAtPath(required_size,
      bf::path(context_->root_application_path.get()))) {
    LOG(ERROR) << "There is not enough space to install application files";
    return Step::Status::OUT_OF_SPACE;
  }

  if (!utils::ExtractToTmpDir(context_->file_path.get().string().c_str(),
      tmp_dir)) {
    LOG(ERROR) << "Failed to process unpack step";
    return Step::Status::ERROR;
  }
  context_->unpacked_dir_path.set(tmp_dir);

  LOG(INFO) << context_->file_path.get() << " was successfully unzipped into "
      << context_->unpacked_dir_path.get();
  return Status::OK;
}

Step::Status StepUnzip::undo() {
  if (access(context_->unpacked_dir_path.get().string().c_str(), F_OK) == 0) {
    bf::remove_all(context_->unpacked_dir_path.get());
    LOG(DEBUG) << "remove temp dir: " << context_->unpacked_dir_path.get();
  }
  return Status::OK;
}


}  // namespace unzip
}  // namespace common_installer
