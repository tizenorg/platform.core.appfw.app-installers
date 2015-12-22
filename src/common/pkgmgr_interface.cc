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
    LOG(ERROR) << "Cannot create pkgmgr_installer object";
    return ENOMEM;
  }

  int result = pkgmgr_installer_receive_request(pi_, argc, argv);
  if (result) {
    LOG(ERROR) << "Cannot receive request. Invalid arguments?";
    // no need to free pkgmgr_installer here. it will be freed in DTOR.
  }

  if (pkgmgr_installer_get_request_type(pi_)
        == PKGMGR_REQ_MANIFEST_DIRECT_INSTALL) {
    /* Add restrictions for manifest direct install
      * - Directory should be located under /usr/apps/
      * - XML path should be located under /usr/share/packages/
      * - Directory name and xml name should be same */
    bf::path directory_path = pkgmgr_installer_get_directory_path(pi_);
    bf::path xml_path = pkgmgr_installer_get_xml_path(pi_);

    if (directory_path.empty() ||
        !bf::is_directory(directory_path) ||
        xml_path.empty() ||
        !bf::is_regular_file(xml_path)) {
      LOG(ERROR) << "invalid parameter";
      return EINVAL;
    }

    if (directory_path.parent_path().compare("/usr/apps") != 0) {
      LOG(ERROR) << "invalid directory path";
      return EINVAL;
    }

    if (xml_path.parent_path().compare("/usr/share/packages") != 0) {
      LOG(ERROR) << "invalid xml path";
      return EINVAL;
    }

    if (directory_path.filename().string()
          .compare(xml_path.stem().string()) != 0) {
      LOG(ERROR) << "invalid parameter: directory path "
          << directory_path
          << "xml path"
          << xml_path;
      return EINVAL;
    }

    // pkgid should be exists in preload app list
    std::ifstream preload_list("/etc/package-manager/preload/preload_list.txt");
    bool is_preload_app = false;
    for (std::string str; std::getline(preload_list, str); ) {
      if (str.compare(directory_path.filename().string()) == 0) {
        is_preload_app = true;
        break;
      }
    }
    preload_list.close();

    if (!is_preload_app) {
      LOG(ERROR) << "Only preload app could be installed by manifest direct install";
      return EINVAL;
    }
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

boost::filesystem::path PkgMgrInterface::GetXMLPath() {
  return boost::filesystem::path(pkgmgr_installer_get_xml_path(pi_));
}

boost::filesystem::path PkgMgrInterface::GetDirectoryPath() {
  return boost::filesystem::path(pkgmgr_installer_get_directory_path(pi_));
}

}  // namespace common_installer
