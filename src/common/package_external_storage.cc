// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/package_external_storage.h"

#include <glib.h>

#include "common/utils/byte_size_literals.h"
#include "common/utils/file_util.h"
#include "common/utils/logging.h"

namespace {

const char kExternalDir[] = "res";

}  // namespace

namespace common_installer {

PackageExternalStorage::PackageExternalStorage(RequestType type,
    const std::string& pkgid, const boost::filesystem::path& application_root)
    : PackageStorage(type, pkgid, application_root),
      handle_(0) {
}

PackageExternalStorage::~PackageExternalStorage() {
  if (handle_)
    app2ext_deinit(handle_);
}

bool PackageExternalStorage::Finilize(bool success) {
  int ret = APP2EXT_STATUS_SUCCESS;
  switch (type_) {
  case RequestType::Install: {
    ret = handle_->interface.post_install(pkgid_.c_str(),
        success ? APP2EXT_STATUS_SUCCESS : APP2EXT_STATUS_FAILED);
    break;
  }
  case RequestType::Update: {
    ret = handle_->interface.post_upgrade(pkgid_.c_str(),
        success ? APP2EXT_STATUS_SUCCESS : APP2EXT_STATUS_FAILED);
    break;
  }
  case RequestType::Uninstall: {
    ret = handle_->interface.post_uninstall(pkgid_.c_str());
    break;
  }
  default:
    assert(false && "Not supported installation mode");
  }
  return ret == APP2EXT_STATUS_SUCCESS;
}

bool PackageExternalStorage::Commit() {
  return Finilize(true);
}

bool PackageExternalStorage::Abort() {
  return Finilize(false);
}

bool PackageExternalStorage::Initialize(
    const boost::filesystem::path& space_requirement) {
  handle_ = app2ext_init(APP2EXT_SD_CARD);
  if (!handle_) {
    LOG(ERROR) << "app2ext_init() failed";
    return false;
  }

  // external size in MB
  int external_size = (GetSize(space_requirement / kExternalDir) + 1_MB) / 1_MB;

  // only res/ directory is installed externally
  app2ext_dir_details* dir_detail = reinterpret_cast<app2ext_dir_details*>(
      calloc(1, sizeof(app2ext_dir_details)));
  dir_detail->name = const_cast<char*>(kExternalDir);
  dir_detail->type = APP2EXT_DIR_RO;
  GList* glist = nullptr;
  glist = g_list_append(glist, dir_detail);

  int ret = 0;
  switch (type_) {
  case RequestType::Install: {
    ret = handle_->interface.pre_install(pkgid_.c_str(), glist, external_size);
    break;
  }
  case RequestType::Update: {
    ret = handle_->interface.pre_upgrade(pkgid_.c_str(), glist, external_size);
    break;
  }
  case RequestType::Uninstall: {
    ret = handle_->interface.pre_uninstall(pkgid_.c_str());
    break;
  }
  case RequestType::Reinstall: {
    LOG(ERROR) << "Reinstall mode is not supported for external installation";
    ret = -1;
    break;
  }
  case RequestType::Recovery: {
    LOG(ERROR) << "Recovery mode is not supported for external installation";
    ret = -1;
    break;
  }
  default: {
    assert(false && "Invalid installation mode");
  }
  }

  free(dir_detail);
  g_list_free(glist);
  return ret == 0;
}

bool PackageExternalStorage::IsRequestedPackageInstalled() const {
  int ret = app2ext_get_app_location(pkgid_.c_str());
  switch (ret) {
  case APP2EXT_SD_CARD:
    return true;
  default:
    return false;
  }
}

}  // namespace common_installer
