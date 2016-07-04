/* 2016, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_change_owner.h"

#include <unistd.h>
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#include <fcntl.h>
#include <pkgmgr-info.h>
#include <cassert>

#include <cstring>
#include <string>

#include "common/utils/file_util.h"
#include "common/utils/glist_range.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace {

const int32_t kPWBufSize = sysconf(_SC_GETPW_R_SIZE_MAX);
const int32_t kGRBufSize = sysconf(_SC_GETGR_R_SIZE_MAX);

bool SetOwner(const bf::path& subpath, uid_t uid, gid_t gid) {
  int fd = open(subpath.c_str(), O_RDONLY);
  if (fd < 0) {
    LOG(ERROR) << "Can't open directory : " << subpath;
    return false;
  }

  int ret = fchown(fd, uid, gid);
  close(fd);
  if (ret != 0) {
    LOG(ERROR) << "Failed to change owner of: " << subpath;
    return false;
  }
  return true;
}

bool SetOwnerForSymbolic(const bf::path& subpath, uid_t uid, gid_t gid) {
  int ret = lchown(subpath.c_str(), uid, gid);
  if (ret != 0) {
    LOG(ERROR) << "Failed to change owner of: " << subpath;
    return false;
  }
  return true;
}

}  // namespace

namespace common_installer {
namespace filesystem {

Step::Status StepChangeOwner::precheck() {
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
    return Step::Status::PACKAGE_NOT_FOUND;
  }

  return Step::Status::OK;
}

Step::Status StepChangeOwner::process() {
  struct passwd pwd;
  struct passwd* pwd_result;
  char buf_pw[kPWBufSize];

  int ret = getpwuid_r(context_->uid.get(), &pwd, buf_pw,
      sizeof(buf_pw), &pwd_result);
  if (ret != 0 || pwd_result == nullptr) {
    LOG(WARNING) << "Failed to get user for home directory: " <<
        context_->uid.get();
    return Status::ERROR;
  }

  struct group gr;
  struct group* gr_result;
  char buf_gr[kGRBufSize];
  ret = getgrgid_r(pwd.pw_gid, &gr, buf_gr, sizeof(buf_gr), &gr_result);
  if (ret != 0)
    return Status::ERROR;

  // Change owner of files at root path
  bf::path start_path = context_->pkg_path.get();
  if (!SetOwner(start_path, pwd.pw_uid, pwd.pw_gid))
    return Status::ERROR;
  for (bf::recursive_directory_iterator iter(start_path);
      iter != bf::recursive_directory_iterator(); ++iter) {
        if (!SetOwner(iter->path(), pwd.pw_uid, pwd.pw_gid))
          return Status::ERROR;
  }

  // For symbolic links
  start_path = context_->pkg_path.get() / bf::path("bin");
  for (bf::recursive_directory_iterator iter(start_path);
      iter != bf::recursive_directory_iterator(); ++iter) {
        if (!SetOwnerForSymbolic(iter->path(), pwd.pw_uid, pwd.pw_gid))
          return Status::ERROR;
  }

  // For icon files
  start_path = getIconPath(context_->uid.get(),
      context_->is_preload_request.get());
  for (application_x* app :
      GListRange<application_x*>(context_->manifest_data.get()->application)) {
    if (app->icon) {
      icon_x* icon = reinterpret_cast<icon_x*>(app->icon->data);
      bf::path source(icon->text);
      if (bf::exists(source)) {
        bf::path destination_path = start_path / app->appid;
        if (source.has_extension())
          destination_path += source.extension();
        else
          destination_path += ".png";

        if (!SetOwner(destination_path, pwd.pw_uid, pwd.pw_gid))
          return Status::ERROR;
      }
    }
  }

  // Manifest files for global apps
  if (!context_->xml_path.get().empty()) {
    if (!SetOwner(context_->xml_path.get(), pwd.pw_uid, pwd.pw_gid))
      return Status::ERROR;
  }

  return Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer
