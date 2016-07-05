// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/external_storage.h"

#include <glib.h>

#include <manifest_parser/utils/logging.h>

#include "common/utils/byte_size_literals.h"
#include "common/utils/file_util.h"

namespace bf = boost::filesystem;

namespace {

const char kWgtType[] = "wgt";
const char kExternalDirForWgt[] = "res";

const std::vector<std::string> kExternalDirsForTpk = {
  "bin",
  "lib",
  "res"
};

int64_t SizeInMB(int64_t size) {
  return (size + 1_MB - 1) / 1_MB;
}

}  // namespace

namespace common_installer {

ExternalStorage::ExternalStorage(RequestType type,
    const std::string& pkgid, const std::string& package_type,
    const boost::filesystem::path& application_root, uid_t uid)
    : type_(type),
      pkgid_(pkgid),
      package_type_(package_type),
      application_root_(application_root),
      uid_(uid),
      handle_(nullptr) {
  if (package_type_ == kWgtType) {
    external_dirs_.push_back(kExternalDirForWgt);
  } else {
    external_dirs_ = kExternalDirsForTpk;
  }
  move_type_ = -1;
}

ExternalStorage::ExternalStorage(RequestType type,
    const std::string& pkgid, const std::string& package_type,
    const boost::filesystem::path& application_root, uid_t uid,
    bool is_external_move)
    : type_(type),
      pkgid_(pkgid),
      package_type_(package_type),
      application_root_(application_root),
      uid_(uid),
      handle_(nullptr) {
  if (package_type_ == kWgtType) {
    external_dirs_.push_back(kExternalDirForWgt);
  } else {
    external_dirs_ = kExternalDirsForTpk;
  }
  if (is_external_move)
    move_type_ = APP2EXT_MOVE_TO_EXT;
  else
    move_type_ = APP2EXT_MOVE_TO_PHONE;
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
  case RequestType::Move: {
    ret = handle_->interface.client_usr_post_move(pkgid_.c_str(),
                               static_cast<app2ext_move_type_t>(move_type_),
                               uid_);
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

const std::vector<std::string>& ExternalStorage::external_dirs() const {
  return external_dirs_;
}

bool ExternalStorage::Initialize(
    const boost::filesystem::path& space_requirement) {
  // external size in MB, set any-non zero size as default
  int external_size = 1;

  if (!space_requirement.empty()) {
    if (package_type_ == kWgtType) {
      for (auto& dir : kExternalDirsForTpk) {
        bf::path requirement = space_requirement / dir;
        if (!bf::exists(requirement))
          continue;
        external_size +=
            SizeInMB(GetDirectorySize(requirement));
      }
    } else {
      // for wgt whole content of package goes to res/
      external_size =
          SizeInMB(GetDirectorySize(space_requirement));
    }
  }

  if (external_size == 0)
    external_size = 1;

  handle_ = app2ext_init(APP2EXT_SD_CARD);
  if (!handle_) {
    LOG(ERROR) << "app2ext_init() failed";
    return false;
  }

  GList* glist = nullptr;
  for (auto& dir : external_dirs_) {
    app2ext_dir_details* dir_detail = reinterpret_cast<app2ext_dir_details*>(
        calloc(1, sizeof(app2ext_dir_details)));
    dir_detail->name = strdup(dir.c_str());
    dir_detail->type = APP2EXT_DIR_RO;
    glist = g_list_append(glist, dir_detail);
  }

  int ret = 0;
  switch (type_) {
  case RequestType::Install:
    ret = handle_->interface.client_usr_force_clean(pkgid_.c_str(), uid_);
    if (ret) {
      LOG(ERROR) << "force_clean failed";
      break;
    }
    ret = handle_->interface.client_usr_pre_install(pkgid_.c_str(), glist,
                                                    external_size, uid_);
    break;
  case RequestType::Update:
  case RequestType::Delta:
    ret = handle_->interface.client_usr_pre_upgrade(pkgid_.c_str(), glist,
                                                    external_size, uid_);
    break;
  case RequestType::Move:
    if (move_type_ == -1) {
      LOG(ERROR) << "Invalid request [" << move_type_ << "]";
      ret = -1;
      break;
    }

    // try umount before move
    ret = handle_->interface.client_usr_disable(pkgid_.c_str(), uid_);

    ret = handle_->interface.client_usr_pre_move(pkgid_.c_str(), glist,
                               static_cast<app2ext_move_type_t>(move_type_),
                               uid_);
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

  g_list_free_full(glist, [](gpointer data) {
      app2ext_dir_details* dir_detail =
          reinterpret_cast<app2ext_dir_details*>(data);
      free(dir_detail->name);
      free(dir_detail);
    });
  return ret == 0;
}

std::unique_ptr<ExternalStorage> ExternalStorage::MoveInstalledStorage(
    RequestType type, const boost::filesystem::path& application_root,
    const std::string& pkgid, const std::string& package_type,
    uid_t uid, bool is_external_move) {

  std::unique_ptr<ExternalStorage> external_storage(
    new ExternalStorage(type, pkgid, package_type, application_root, uid,
                       is_external_move));
  if (!external_storage->Initialize(application_root)) {
    LOG(WARNING) << "Cannot initialize external storage for move";
    return nullptr;
  }

  return external_storage;
}

std::unique_ptr<ExternalStorage> ExternalStorage::AcquireExternalStorage(
    RequestType type, const boost::filesystem::path& application_root,
    const std::string& pkgid, const std::string& package_type,
    const boost::filesystem::path& space_requirement,
    uid_t uid) {
  std::unique_ptr<ExternalStorage> external_storage(
      new ExternalStorage(type, pkgid, package_type, application_root, uid));
  if (!external_storage->Initialize(space_requirement)) {
    LOG(WARNING) << "Cannot initialize external storage for request";
    return nullptr;
  }
  return external_storage;
}

}  // namespace common_installer
