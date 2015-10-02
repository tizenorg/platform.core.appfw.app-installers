// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/external_storage.h"

#include <glib.h>

#include <manifest_parser/utils/logging.h>

#include "common/utils/byte_size_literals.h"
#include "common/utils/file_util.h"

namespace {

const char kExternalDir[] = "res";

int64_t SizeInMB(int64_t size) {
  return (size + 1_MB - 1) / 1_MB;
}

}  // namespace

namespace common_installer {

ExternalStorage::ExternalStorage(RequestType type,
    const std::string& pkgid, const boost::filesystem::path& application_root,
    uid_t uid)
    : type_(type),
      pkgid_(pkgid),
      application_root_(application_root),
      uid_(uid),
      handle_(nullptr) {
}

ExternalStorage::~ExternalStorage() {
  if (handle_)
    app2ext_deinit(handle_);
}

bool ExternalStorage::Finalize(bool success) {
  int ret = APP2EXT_STATUS_SUCCESS;
  switch (type_) {
  case RequestType::Install: {
    ret = handle_->interface.client_usr_post_install(pkgid_.c_str(),
        success ? APP2EXT_STATUS_SUCCESS : APP2EXT_STATUS_FAILED, uid_);
    break;
  }
  case RequestType::Update: {
    ret = handle_->interface.client_usr_post_upgrade(pkgid_.c_str(),
        success ? APP2EXT_STATUS_SUCCESS : APP2EXT_STATUS_FAILED, uid_);
    break;
  }
  case RequestType::Uninstall: {
    ret = handle_->interface.client_usr_post_uninstall(pkgid_.c_str(), uid_);
    break;
  }
  default:
    assert(false && "Not supported installation mode");
  }
  return ret == APP2EXT_STATUS_SUCCESS;
}

bool ExternalStorage::Commit() {
  return Finalize(true);
}

bool ExternalStorage::Abort() {
  return Finalize(false);
}

bool ExternalStorage::Initialize(
    const boost::filesystem::path& space_requirement) {
  // external size in MB, set any-non zero size as default
  int external_size = 1;

  if (!space_requirement.empty()) {
    external_size =
        SizeInMB(GetDirectorySize(space_requirement / kExternalDir));
  }

  if (external_size == 0)
    external_size = 1;

  handle_ = app2ext_init(APP2EXT_SD_CARD);
  if (!handle_) {
    LOG(ERROR) << "app2ext_init() failed";
    return false;
  }

  // only res/ directory is installed externally
  app2ext_dir_details* dir_detail = reinterpret_cast<app2ext_dir_details*>(
      calloc(1, sizeof(app2ext_dir_details)));
  dir_detail->name = strdup(kExternalDir);
  dir_detail->type = APP2EXT_DIR_RO;
  GList* glist = g_list_append(nullptr, dir_detail);

  int ret = 0;
  switch (type_) {
  case RequestType::Install:
    ret = handle_->interface.client_usr_pre_install(pkgid_.c_str(), glist,
                                                external_size, uid_);
    break;
  case RequestType::Update:
  case RequestType::Delta:
    ret = handle_->interface.client_usr_pre_upgrade(pkgid_.c_str(), glist,
                                                external_size, uid_);
    break;
  case RequestType::Uninstall:
    ret = handle_->interface.client_usr_pre_uninstall(pkgid_.c_str(), uid_);
    break;
  case RequestType::Clear:
  case RequestType::Reinstall:
  case RequestType::Recovery:
  case RequestType::ManifestDirectInstall:
  case RequestType::ManifestDirectUpdate:
  case RequestType::MountInstall:
  case RequestType::MountUpdate:
    LOG(ERROR) << "Installation type is not supported by external installation";
    ret = -1;
    break;
  default:
    assert(false && "Invalid installation mode");
  }

  free(dir_detail->name);
  free(dir_detail);
  g_list_free(glist);
  return ret == 0;
}

std::unique_ptr<ExternalStorage> ExternalStorage::AcquireExternalStorage(
    RequestType type, const boost::filesystem::path& application_root,
    const std::string& pkgid, const boost::filesystem::path& space_requirement,
    uid_t uid) {
  std::unique_ptr<ExternalStorage> external_storage(
      new ExternalStorage(type, pkgid, application_root, uid));
  if (!external_storage->Initialize(space_requirement)) {
    LOG(WARNING) << "Cannot initialize external storage for request";
    return nullptr;
  }
  return external_storage;
}

}  // namespace common_installer
