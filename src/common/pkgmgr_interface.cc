// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/pkgmgr_interface.h"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>


#include <memory>
#include <string>
#include <fstream>

#include "common/app_query_interface.h"

namespace bf = boost::filesystem;

namespace {

const char kDeltaFileExtension[] = ".delta";

}

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
    LOG(WARNING) << "Cannot create pkgmgr_installer object. Will try offline";
    // TODO(t.iwanek): app-installer should recognize offline installation and
    // this information should be accesible in installation context
    pi_ = pkgmgr_installer_offline_new();
    if (!pi_) {
      LOG(ERROR) << "Cannot create pkgmgr_installer object. Aborting.";
      return ENOMEM;
    }
    install_mode_ = InstallationMode::OFFLINE;
  } else {
    install_mode_ = InstallationMode::ONLINE;
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
    case PKGMGR_REQ_INSTALL : {
      auto request_info = GetRequestInfo();
      if (!request_info)
        return RequestType::Unknown;
      std::string extension = bf::path(request_info).extension().string();
      if (!is_app_installed_) {
        if (extension == kDeltaFileExtension) {
          LOG(ERROR) << "Package is not installed. "
                        "Cannot update from delta package";
          return RequestType::Unknown;
        } else {
          return RequestType::Install;
        }
      } else {
        if (extension == kDeltaFileExtension)
          return RequestType::Delta;
        else
          return RequestType::Update;
      }
    }
    case PKGMGR_REQ_UNINSTALL:
      return RequestType::Uninstall;
    case PKGMGR_REQ_REINSTALL:
      return RequestType::Reinstall;
    case PKGMGR_REQ_CLEAR:
      return RequestType::Clear;
    case PKGMGR_REQ_RECOVER:
      return RequestType::Recovery;
    case PKGMGR_REQ_MANIFEST_DIRECT_INSTALL:
      if (!is_app_installed_)
        return RequestType::ManifestDirectInstall;
      else
        return RequestType::ManifestDirectUpdate;
    default:
      return RequestType::Unknown;
  }
}

const char* PkgMgrInterface::GetRequestInfo() const {
  return pkgmgr_installer_get_request_info(pi_);
}

boost::filesystem::path PkgMgrInterface::GetTepPath() const {
  if (pkgmgr_installer_get_tep_path(pi_) == nullptr)
    return boost::filesystem::path("");
  else
    return boost::filesystem::path(pkgmgr_installer_get_tep_path(pi_));
}

bool PkgMgrInterface::GetIsTepMove() {
  return (pkgmgr_installer_get_tep_move_type(pi_) == 1)?true:false;
}

}  // namespace common_installer
