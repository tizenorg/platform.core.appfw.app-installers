// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_delta_patch.h"

#include <boost/system/error_code.hpp>
#include <boost/filesystem/path.hpp>
#include <delta/delta_handler.h>
#include <delta/delta_parser.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdlib>

#include "common/utils/file_util.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;
namespace ci = common_installer;

namespace {

const char kBinaryDir[] = "bin";
const char kCacheDir[] = "cache";
const char kDataDir[] = "data";
const char kSharedData[] = "shared/data";
const char kSharedTrusted[] = "shared/trusted";

const char kDeltaFile[] = "delta_info.xml";
const char kXDeltaBinary[] = "/usr/bin/xdelta3";

bool ValidateDeltaInfo(const delta::DeltaInfo& info) {
  for (auto& item : info.added()) {
    if (ci::HasDirectoryClimbing(item))
      return false;
  }
  for (auto& item : info.modified()) {
    if (ci::HasDirectoryClimbing(item))
      return false;
  }
  for (auto& item : info.removed()) {
    if (ci::HasDirectoryClimbing(item))
      return false;
  }
  return true;
}

void RemoveBinarySymlinks(const bf::path& dir) {
  for (bf::directory_iterator iter(dir / kBinaryDir);
      iter != bf::directory_iterator(); ++iter) {
    if (bf::is_symlink(iter->path())) {
      // FIXME: note that this assumes that it is not possible to create
      // explicitly symlinks in bin/ directory pointing to whatever
      bs::error_code error;
      bf::remove(iter->path(), error);
    }
  }
}

void RemoveStorageDirectories(const bf::path& dir) {
  bs::error_code error;
  bf::remove_all(dir / kDataDir, error);
  bf::remove_all(dir / kCacheDir, error);
  bf::remove_all(dir / kSharedData, error);
  bf::remove_all(dir / kSharedTrusted, error);
}

bool ApplyDeletedFiles(const delta::DeltaInfo& info, const bf::path& app_dir) {
  for (auto& relative : info.removed()) {
    bs::error_code error;
    bf::remove_all(app_dir / relative, error);
    if (error) {
      LOG(WARNING) << "Failed to remove";
    }
    LOG(DEBUG) << "Deleted: " << relative;
  }
  return true;
}

bool ApplyModifiedFiles(const delta::DeltaInfo& info, const bf::path& app_dir,
                        const bf::path& patch_dir) {
  for (auto& relative : info.modified()) {
    bf::path temp_file = ci::GenerateTemporaryPath(
        bf::path(ci::GetRootAppPath()) / "tmp_file");
    bf::path patch_file = patch_dir / relative;
    bf::path input = app_dir / relative;
    if (!bf::is_regular_file(input)) {
      LOG(ERROR) << "Cannot modify. Not a regular file: " << input;
      return false;
    }
    const char* const argv[] = {
      kXDeltaBinary,
      "-d",
      "-s",
      input.c_str(),
      patch_file.c_str(),
      temp_file.c_str(),
      nullptr,
    };
    pid_t pid = fork();
    if (pid == 0) {
    int ret = execv(argv[0], const_cast<char* const*>(argv));
      if (ret != 0) {
        // no other thing to -> do just quit
        exit(-1);
      }
    } else if (pid == -1) {
      LOG(ERROR) << "Failed to fork";
      return false;
    } else {
      int status;
      waitpid(pid, &status, 0);
      if (status != 0) {
        LOG(ERROR) << "xdelta3 failed with error code: " << status;
        return false;
      }
    }
    bs::error_code error;
    bf::copy_file(temp_file, input, bf::copy_option::overwrite_if_exists,
                  error);
    if (error) {
      LOG(ERROR) << "Failed to copy from " << temp_file << " to " << input;
      bf::remove(temp_file, error);
      return false;
    }
    bf::remove(temp_file, error);
    LOG(DEBUG) << "Patched: " << relative;
  }
  return true;
}

bool ApplyAddedFiles(const delta::DeltaInfo& info, const bf::path& app_dir,
                     const bf::path& patch_dir) {
  for (auto& relative : info.added()) {
    bf::path source = patch_dir / relative;
    bf::path target = app_dir / relative;
    bs::error_code error;
    if (bf::is_directory(source)) {
      bf::create_directories(target, error);
      if (error) {
        LOG(ERROR) << "Failed to add: " << relative;
        return false;
      }
    } else {
      if (bf::exists(target)) {
        bf::remove(target, error);
        if (error) {
          LOG(ERROR) << "Cannot remove file: " << target;
          return false;
        }
      }
      bf::create_directories(target.parent_path(), error);
      if (!ci::MoveFile(source, target)) {
        LOG(ERROR) << "Failed to move file: " << source << " to " << target;
        return false;
      }
    }
    LOG(DEBUG) << "Added: " << relative;
  }
  return true;
}

bool ApplyPatch(const delta::DeltaInfo& info, const bf::path& app_dir,
                const bf::path& patch_dir) {
  if (!ApplyDeletedFiles(info, app_dir))
    return false;
  if (!ApplyModifiedFiles(info, app_dir, patch_dir))
    return false;
  if (!ApplyAddedFiles(info, app_dir, patch_dir))
    return false;
  return true;
}

}  // namespace

namespace common_installer {
namespace filesystem {

StepDeltaPatch::StepDeltaPatch(InstallerContext* context,
                               const std::string& delta_root)
    : Step(context),
      delta_root_(delta_root) {
}

Step::Status StepDeltaPatch::precheck() {
  if (context_->unpacked_dir_path.get().empty()) {
    LOG(ERROR) << "Unpacked dir is not set";
    return Status::ERROR;
  }
  if (context_->pkgid.get().empty()) {
    LOG(ERROR) << "Package id is not set";
    return Status::ERROR;
  }
  return Status::OK;
}

Step::Status StepDeltaPatch::process() {
  bf::path delta_file = context_->unpacked_dir_path.get() / kDeltaFile;
  if (!bf::exists(delta_file)) {
    LOG(ERROR) << "Delta file doesn't exist in package.";
    return Status::ERROR;
  }
  delta::DeltaParser parser;
  if (!parser.ParseManifest(delta_file)) {
    LOG(ERROR) << parser.GetErrorMessage();
    return Status::ERROR;
  }
  std::shared_ptr<const delta::DeltaInfo> delta_info =
      std::static_pointer_cast<const delta::DeltaInfo>(
        parser.GetManifestData(delta::kDeltaInfoKey));
  if (!delta_info) {
    LOG(ERROR) << "Failed to parse delta information";
    return Status::ERROR;
  }

  // additional validation
  if (!ValidateDeltaInfo(*delta_info)) {
    LOG(ERROR) << "Delta info is malformed";
    return Status::ERROR;
  }

  // create old content directory and patch directory
  patch_dir_ = context_->unpacked_dir_path.get();
  patch_dir_ += ".patch";
  if (!MoveDir(context_->unpacked_dir_path.get(), patch_dir_)) {
    LOG(ERROR) << "Failed to move content to patch directory";
    return Status::ERROR;
  }

  if (!CopyDir(context_->root_application_path.get() / context_->pkgid.get()
               / delta_root_, context_->unpacked_dir_path.get())) {
    LOG(ERROR) << "Failed to copy package files";
    return Status::ERROR;
  }

  // if there is no root set, that means we need to handle files added by
  // installer itself (during installation) and files added during runtime
  // they will be restored in process so just remove extra copy here, so that
  // it doesn't interfere with installation process
  if (delta_root_.empty()) {
    RemoveBinarySymlinks(context_->unpacked_dir_path.get());
    RemoveStorageDirectories(context_->unpacked_dir_path.get());
  }

  // apply changes mentioned in delta
  if (!ApplyPatch(*delta_info, context_->unpacked_dir_path.get(), patch_dir_))
    return Status::ERROR;

  bs::error_code error;
  bf::remove_all(patch_dir_, error);
  LOG(INFO) << "Delta patch applied successfully";
  return Status::OK;
}

Step::Status StepDeltaPatch::undo() {
  bs::error_code error;
  bf::remove_all(patch_dir_, error);
  return Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer
