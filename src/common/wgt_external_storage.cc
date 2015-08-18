/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/utils/logging.h"
#include "common/wgt_external_storage.h"

namespace common_installer {

const char* GLIST_RES_DIR = "res";

WgtExternalStorage::WgtExternalStorage(const std::string& pkgid) {
  LOG(DEBUG) << "WgtExternalStorage initialization started";
  pkgid_ = pkgid;
  app2_handle_ = app2ext_init(APP2EXT_SD_CARD);
  if (app2_handle_) {
    LOG(DEBUG) << "App2Ext initialization success";
    initialization_ = true;
  } else {
    LOG(ERROR) << "App2Ext initialization failed";
    initialization_ = false;
  }
}

WgtExternalStorage::~WgtExternalStorage() {
  LOG(DEBUG) << "WgtExternalStorage deinitialization started";
  if (!IsInitialized()) {
    return;
  }
  int ret = app2ext_deinit(app2_handle_);
    switch (ret) {
    case APP2EXT_STATUS_SUCCESS:
      LOG(DEBUG) << "App2Ext deinitialization success";
      break;
    case APP2EXT_STATUS_FAILED:
      LOG(ERROR) << "App2Ext deinitialization failed";
      break;
    default:
      LOG(ERROR) << "App2Ext deinitialization error";
      break;
  }
}

bool WgtExternalStorage::IsInitialized() {
  return initialization_;
}

bool WgtExternalStorage::PostInstallation(bool execute) {
  LOG(DEBUG) << "WgtExternalStorage postinstallation started";
  app2ext_status_t status;
  if (execute)
    status = APP2EXT_STATUS_SUCCESS;
  else
    status = APP2EXT_STATUS_FAILED;
  int ret = app2_handle_->interface.post_install(pkgid_.c_str(), status);
  switch (ret) {
  case APP2EXT_STATUS_SUCCESS:
    LOG(DEBUG) << "App2Ext postinstallation success";
    return true;
  case APP2EXT_STATUS_FAILED:
    LOG(ERROR) << "App2Ext postinstallation failed";
    return false;
  default:
    LOG(ERROR) << "App2Ext postinstallation error";
    return false;
  }
}

bool WgtExternalStorage::PostUninstallation() {
  LOG(DEBUG) << "WgtExternalStorage postuninstallation started";
  int ret = app2_handle_->interface.post_uninstall(pkgid_.c_str());
  switch (ret) {
  case APP2EXT_STATUS_SUCCESS:
    LOG(DEBUG) << "App2Ext postuninstallation success";
    return true;
  case APP2EXT_STATUS_FAILED:
    LOG(ERROR) << "App2Ext postuninstallation failed";
    return false;
  default:
    LOG(ERROR) << "App2Ext postuninstallation error";
    return false;
  }
}

bool WgtExternalStorage::PreUninstallation() {
  LOG(DEBUG) << "WgtExternalStorage preuninstallation started";
  int ret = app2_handle_->interface.pre_uninstall(pkgid_.c_str());
  switch (ret) {
  case APP2EXT_STATUS_SUCCESS:
    LOG(DEBUG) << "App2Ext preuninstallation success";
    return true;
  case APP2EXT_STATUS_FAILED:
    LOG(ERROR) << "App2Ext preuninstallation failed";
    return false;
  default:
    LOG(ERROR) << "App2Ext postuninstallation error";
    return false;
  }
}

bool WgtExternalStorage::PreInstallation() {
  LOG(DEBUG) << "WgtExternalStorage preinstallation started";
  int size = 0;
  GList *dir_list = nullptr;
  app2ext_dir_details* dirDetail = nullptr;
  dirDetail = reinterpret_cast<app2ext_dir_details*>
      (calloc(1, sizeof(app2ext_dir_details)));
  if (!dirDetail) {
    return false;
  }
  dirDetail->name = strdup(GLIST_RES_DIR);
  dirDetail->type = APP2EXT_DIR_RO;
  dir_list = g_list_append(dir_list, dirDetail);
  // TODO(a.niznik) SMACK/SECURITY rules must be checked in case of such
  // external installation.
  int ret = app2_handle_->interface.pre_install(pkgid_.c_str(), dir_list, size);
  switch (ret) {
  case APP2EXT_STATUS_SUCCESS:
    LOG(DEBUG) << "App2Ext preinstallation success";
    free(dirDetail);
    g_list_free(dir_list);
    return true;
  case APP2EXT_STATUS_FAILED:
    LOG(ERROR) << "App2Ext preinstallation failed";
    return false;
  default:
    LOG(ERROR) << "App2Ext preinstallation error";
    return false;
  }
}

}  // namespace common_installer
