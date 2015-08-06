// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/pkgmgr_interface.h"

#include <memory>

#include "common/app_query_interface.h"

namespace common_installer {

PkgMgrPtr PkgMgrInterface::Create(int argc, char** argv,
                                  AppQueryInterface* interface) {
  PkgMgrPtr instance(new PkgMgrInterface(interface));
  int result = instance->InitInternal(argc, argv);
  if (result != 0)
    return nullptr;

  return instance;
}

int PkgMgrInterface::InitInternal(int argc, char** argv) {
  pi_ = pkgmgr_installer_new();

  if (!pi_) {
    LOG(ERROR) << "Cannot create pkgmgr_installer object";
    return ENOMEM;
  }

  int result = pkgmgr_installer_receive_request(pi_, argc, argv);
  if (result) {
    LOG(ERROR) << "Cannot receive request. Invalid arguments?";
    // no need to free pkgmgr_installer here. it will be freed in DTOR.
  }

  is_app_installed_ = false;
  if (query_interface_)
    is_app_installed_ = query_interface_->IsAppInstalledByArgv(argc, argv);

  return result;
}

PkgMgrInterface::~PkgMgrInterface() {
  if (pi_)
    pkgmgr_installer_free(pi_);
}

RequestType PkgMgrInterface::GetRequestType() const {
  switch (pkgmgr_installer_get_request_type(pi_)) {
    case PKGMGR_REQ_INSTALL:
      if (!is_app_installed_) {
        return RequestType::Install;
      } else {
        return RequestType::Update;
      }
    case PKGMGR_REQ_UNINSTALL:
      return RequestType::Uninstall;
    case PKGMGR_REQ_REINSTALL:
      return RequestType::Reinstall;
    case PKGMGR_REQ_RECOVER:
      return RequestType::Recovery;
    default:
      return RequestType::Unknown;
  }
}

const char* PkgMgrInterface::GetRequestInfo() const {
  return pkgmgr_installer_get_request_info(pi_);
}

}  // namespace common_installer
