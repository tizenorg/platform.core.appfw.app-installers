// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/pkgmgr_interface.h"

#include <memory>

namespace common_installer {

PkgMgrPtr PkgMgrInterface::instance_;

PkgMgrPtr PkgMgrInterface::Instance() {
    return instance_;
}

int PkgMgrInterface::Init(int argc, char** argv) {
  if (instance_)
      return 0;

  PkgMgrPtr tmp(new PkgMgrInterface());
  int result = tmp->InitInternal(argc, argv);

  if (result) {
      instance_ = std::move(tmp);
  }
  return result;
}

int PkgMgrInterface::InitInternal(int argc, char** argv) {
  pi_ = pkgmgr_installer_new();

  if (!pi_) {
    LOG(ERROR) << "Cannot create pkgmgr_installer object";
    return ENOMEM;
  }

  int result = pkgmgr_installer_receive_request(pi_, argc, argv);
  if (!result) {
    LOG(ERROR) << "Cannot receive request. Invalid arguments?";
    // no need to free pkgmgr_installer here. it will be freed in DTOR.
  }
  return result;
}

PkgMgrInterface::~PkgMgrInterface() {
  if (pi_)
    pkgmgr_installer_free(pi_);
}

PkgMgrInterface::Type PkgMgrInterface::GetRequestType() const {
  switch (pkgmgr_installer_get_request_type(pi_)) {
    case PKGMGR_REQ_INSTALL:
      return PkgMgrInterface::Type::Install;
    case PKGMGR_REQ_UNINSTALL:
      return PkgMgrInterface::Type::Uninstall;
    case PKGMGR_REQ_REINSTALL:
      return PkgMgrInterface::Type::Reinstall;
    default:
      return PkgMgrInterface::Type::Unknown;
  }
}

const char* PkgMgrInterface::GetRequestInfo() const {
  return pkgmgr_installer_get_request_info(pi_);
}

}  // namespace common_installer
