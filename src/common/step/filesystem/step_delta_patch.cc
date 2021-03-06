// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_delta_patch.h"

#include <boost/system/error_code.hpp>
#include <boost/filesystem/path.hpp>
#include <delta/delta_handler.h>
#include <delta/delta_parser.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <cstdlib>

#include "common/utils/file_util.h"
#include "common/utils/glist_range.h"

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

const char kExternalMemoryMountPoint[] = ".mmc";

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

void RemoveExtraIconFiles(const bf::path& dir, const bf::path& pkg_dir,
                          manifest_x* manifest) {
  for (application_x* app : GListRange<application_x*>(manifest->application)) {
    if (strcmp("webapp", app->type) != 0)
      continue;
    auto range = GListRange<icon_x*>(app->icon);
    auto iter = range.begin();
    if (iter != range.end()) {
      bs::error_code error;
      std::string old_path((*iter)->text);
      bf::path icon_copy = dir / old_path.substr(pkg_dir.string().size());
      bf::remove(icon_copy, error);
    }
  }
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
                        const bf::path& patch_dir, bool is_preload) {
  for (auto& relative : info.modified()) {
    bf::path temp_file = ci::GenerateTemporaryPath(
        bf::path(ci::GetRootAppPath(is_preload)) / "tmp_file");
    bf::path patch_file = patch_dir / relative;
    bf::path input = app_dir / relative;
    if (!bf::is_regular_file(input)) {
      LOG(ERROR) << "Cannot modify. Not a regular file: " << input;
      return false;
    }
    pid_t pid = fork();
    if (pid == 0) {
      const char* const argv[] = {
        kXDeltaBinary,
        "-d",
        "-s",
        input.c_str(),
        patch_file.c_str(),
        temp_file.c_str(),
        nullptr,
      };
      int ret = execv(argv[0], const_cast<char* const*>(argv));
      if (ret != 0) {
        // no other thing to -> do just quit
        exit(-1);
      }
    } else if (pid == -1) {
      LOG(ERROR) << "Failed to fork with errno: " << errno;
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
                const bf::path& patch_dir, bool is_preload) {
  if (!ApplyDeletedFiles(info, app_dir))
    return false;
  if (!ApplyModifiedFiles(info, app_dir, patch_dir, is_preload))
    return false;
  if (!ApplyAddedFiles(info, app_dir, patch_dir))
    return false;
  return true;
}

bool CopySkipMount(const bf::path& from, const bf::path& to) {
  bs::error_code error;
  bf::create_directory(to, error);
  if (error) {
    LOG(ERROR) << "Failed to create target directory";
    return false;
  }
  for (bf::directory_iterator iter(from); iter != bf::directory_iterator();
       ++iter) {
    if (iter->path().filename() == kExternalMemoryMountPoint)
      continue;

    if (bf::is_directory(iter->path())) {
      if (!ci::CopyDir(iter->path(), to / iter->path().filename())) {
        LOG(ERROR) << "Failed to create copy of: " << iter->path();
        return false;
      }
    } else {
      bs::error_code error;
      bf::copy(iter->path(), to / iter->path().filename(), error);
      if (error) {
        LOG(ERROR) << "Failed to create copy of: " << iter->path();
        return false;
      }
    }
  }
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
    return Status::INVALID_VALUE;
  }
  if (context_->pkgid.get().empty()) {
    LOG(ERROR) << "Package id is not set";
    return Status::PACKAGE_NOT_FOUND;
  }
  return Status::OK;
}

Step::Status StepDeltaPatch::process() {
  bf::path delta_file = context_->unpacked_dir_path.get() / kDeltaFile;
  if (!bf::exists(delta_file)) {
    LOG(ERROR) << "Delta file doesn't exist in package.";
    return Status::DELTA_ERROR;
  }
  delta::DeltaParser parser;
  if (!parser.ParseManifest(delta_file)) {
    LOG(ERROR) << parser.GetErrorMessage();
    return Status::DELTA_ERROR;
  }
  std::shared_ptr<const delta::DeltaInfo> delta_info =
      std::static_pointer_cast<const delta::DeltaInfo>(
        parser.GetManifestData(delta::kDeltaInfoKey));
  if (!delta_info) {
    LOG(ERROR) << "Failed to parse delta information";
    return Status::DELTA_ERROR;
  }

  // additional validation
  if (!ValidateDeltaInfo(*delta_info)) {
    LOG(ERROR) << "Delta info is malformed";
    return Status::DELTA_ERROR;
  }

  // create old content directory and patch directory
  patch_dir_ = context_->unpacked_dir_path.get();
  patch_dir_ += ".patch";
  if (!MoveDir(context_->unpacked_dir_path.get(), patch_dir_)) {
    LOG(ERROR) << "Failed to move content to patch directory";
    return Status::DELTA_ERROR;
  }

  if (!CopySkipMount(
      context_->root_application_path.get() / context_->pkgid.get()
          / delta_root_,
      context_->unpacked_dir_path.get())) {
    LOG(ERROR) << "Failed to copy package files";
    return Status::DELTA_ERROR;
  }

  // if there is no root set, that means we need to handle files added by
  // installer itself (during installation) and files added during runtime
  // they will be restored in process so just remove extra copy here, so that
  // it doesn't interfere with installation process
  if (delta_root_.empty()) {
    RemoveBinarySymlinks(context_->unpacked_dir_path.get());
    RemoveStorageDirectories(context_->unpacked_dir_path.get());
    // TODO(t.iwanek): subclass this step in wgt backend
    RemoveExtraIconFiles(
        context_->unpacked_dir_path.get(),
        context_->root_application_path.get() / context_->pkgid.get(),
        context_->old_manifest_data.get());
  }

  // apply changes mentioned in delta
  if (!ApplyPatch(*delta_info, context_->unpacked_dir_path.get(), patch_dir_,
    context_->is_preload_request.get()))
    return Status::DELTA_ERROR;

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
