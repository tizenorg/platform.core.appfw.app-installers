// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/pkgmgr_registration.h"

#include <pkgmgr_installer.h>
#include <tzplatform_config.h>
#include <unistd.h>

#include "common/utils/logging.h"

namespace bf = boost::filesystem;

namespace {

// TODO(sdi2): Check if data->removable is correctly setting
// during parsing step.
// Same check should be done for preload field.

// Having a specific step to implement a installer commandline tool
// for image build could be usefull also.
const char* const kAppinstTags[] = {"removable=true", nullptr, };

bool RegisterAuthorCertificate(
    const common_installer::CertificateInfo& cert_info,
    const std::string& pkgid, uid_t uid) {
  pkgmgr_instcertinfo_h handle;
  if (pkgmgr_installer_create_certinfo_set_handle(&handle) < 0) {
    LOG(ERROR) << "Cannot create pkgmgr_instcertinfo_h";
    return false;
  }

  const auto& cert = cert_info.author_certificate.get();

  // TODO(t.iwanek): set other certificates if needed

  if (pkgmgr_installer_set_cert_value(handle, PM_SET_AUTHOR_SIGNER_CERT,
      const_cast<char*>(cert->getBase64().c_str())) < 0) {
    pkgmgr_installer_destroy_certinfo_set_handle(handle);
    LOG(ERROR) << "pkgmgrInstallerSetCertValue fail";
    return false;
  }

  if (pkgmgr_installer_save_certinfo(pkgid.c_str(), handle, uid) < 0) {
    pkgmgr_installer_destroy_certinfo_set_handle(handle);
    LOG(ERROR) << "Failed to save certificate information";
    return false;
  }

  pkgmgr_installer_destroy_certinfo_set_handle(handle);
  return true;
}

}  // anonymous namespace

namespace common_installer {

bool RegisterAppInPkgmgr(const bf::path& xml_path,
                         const std::string& pkgid,
                         const CertificateInfo& cert_info,
                         uid_t uid,
                         const RequestMode& request_mode) {
  int ret = request_mode != RequestMode::GLOBAL ?
      pkgmgr_parser_parse_usr_manifest_for_installation(
          xml_path.c_str(), uid, const_cast<char* const*>(kAppinstTags)) :
      pkgmgr_parser_parse_manifest_for_installation(
          xml_path.c_str(), const_cast<char* const*>(kAppinstTags));
  if (ret) {
    LOG(ERROR) << "Failed to register package: " << xml_path << ", "
        "error code=" << ret;
    return false;
  }

  if (!!cert_info.author_certificate.get()) {
    if (!RegisterAuthorCertificate(cert_info, pkgid, uid)) {
      LOG(ERROR) << "Failed to register author certificate";
      return false;
    }
  }

  return true;
}

bool UpgradeAppInPkgmgr(const bf::path& xml_path, const std::string& pkgid,
                        const CertificateInfo& cert_info, uid_t uid,
                        const RequestMode& request_mode) {
  int ret = request_mode != RequestMode::GLOBAL ?
       pkgmgr_parser_parse_usr_manifest_for_upgrade(
           xml_path.string().c_str(), uid,
           const_cast<char* const*>(kAppinstTags)) :
       pkgmgr_parser_parse_manifest_for_upgrade(
           xml_path.string().c_str(),
           const_cast<char* const*>(kAppinstTags));

  if (ret != 0) {
    LOG(ERROR) << "Failed to upgrade package: " << xml_path;
    return false;
  }

  (void) pkgmgr_installer_delete_certinfo(pkgid.c_str());
  if (!!cert_info.author_certificate.get()) {
    if (!RegisterAuthorCertificate(cert_info, pkgid, uid)) {
      return false;
    }
  }

  return true;
}

bool UnregisterAppInPkgmgr(const bf::path& xml_path,
                           const std::string& pkgid,
                           uid_t uid,
                           const RequestMode& request_mode) {
  int ret = request_mode != RequestMode::GLOBAL ?
      pkgmgr_parser_parse_usr_manifest_for_uninstallation(
          xml_path.string().c_str(), uid,
          const_cast<char* const*>(kAppinstTags)) :
      pkgmgr_parser_parse_manifest_for_uninstallation(
          xml_path.string().c_str(), const_cast<char* const*>(kAppinstTags));
  if (ret) {
    LOG(ERROR) << "Failed to unregister package: " << xml_path;
    return false;
  }

  // Certificate info may be not present
  (void) pkgmgr_installer_delete_certinfo(pkgid.c_str());

  return true;
}

std::string QueryCertificateAuthorCertificate(const std::string& pkgid,
                                              uid_t uid) {
  pkgmgrinfo_certinfo_h handle;
  int ret = pkgmgrinfo_pkginfo_create_certinfo(&handle);
  if (ret != PMINFO_R_OK) {
    LOG(ERROR) << "pkgmgrinfo_pkginfo_create_certinfo failed with error: "
               << ret;
    return {};
  }
  ret = pkgmgrinfo_pkginfo_load_certinfo(pkgid.c_str(), handle, uid);
  if (ret != PMINFO_R_OK) {
    LOG(ERROR) << "pkgmgrinfo_pkginfo_load_certinfo failed with error: " << ret;
    pkgmgrinfo_pkginfo_destroy_certinfo(handle);
    return {};
  }
  const char* author_cert = nullptr;
  ret = pkgmgrinfo_pkginfo_get_cert_value(handle, PMINFO_AUTHOR_SIGNER_CERT,
                                          &author_cert);
  if (ret != PMINFO_R_OK) {
    LOG(ERROR) << "pkgmgrinfo_pkginfo_get_cert_value failed with error: "
               << ret;
    pkgmgrinfo_pkginfo_destroy_certinfo(handle);
    return {};
  }
  std::string old_author_certificate;
  if (author_cert)
    old_author_certificate = author_cert;
  pkgmgrinfo_pkginfo_destroy_certinfo(handle);
  return old_author_certificate;
}

bool IsPackageInstalled(const std::string& pkg_id,
                        const RequestMode& request_mode) {
  pkgmgrinfo_pkginfo_h handle;
  int ret = pkgmgrinfo_pkginfo_get_usr_pkginfo(pkg_id.c_str(), getuid(),
                                               &handle);
  if (ret != PMINFO_R_OK)
    return false;
  bool is_global = false;
  if (pkgmgrinfo_pkginfo_is_for_all_users(handle, &is_global) != PMINFO_R_OK) {
    LOG(ERROR) << "pkgmgrinfo_pkginfo_is_for_all_users failed";
    pkgmgrinfo_pkginfo_destroy_pkginfo(handle);
    return false;
  }
  if (request_mode != RequestMode::GLOBAL && is_global) {
    pkgmgrinfo_pkginfo_destroy_pkginfo(handle);
    return false;
  }

  pkgmgrinfo_pkginfo_destroy_pkginfo(handle);
  return true;
}


}  // namespace common_installer
