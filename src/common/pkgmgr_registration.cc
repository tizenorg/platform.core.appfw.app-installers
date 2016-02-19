// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/pkgmgr_registration.h"

#include <manifest_parser/utils/logging.h>
#include <pkgmgr_installer.h>
#include <pkgmgr_parser_db.h>
#include <tzplatform_config.h>
#include <unistd.h>

namespace bf = boost::filesystem;

namespace {

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

  const auto& im_cert = cert_info.author_intermediate_certificate.get();
  if (pkgmgr_installer_set_cert_value(handle, PM_SET_AUTHOR_INTERMEDIATE_CERT,
      const_cast<char*>(im_cert->getBase64().c_str())) < 0) {
    pkgmgr_installer_destroy_certinfo_set_handle(handle);
    LOG(ERROR) << "pkgmgrInstallerSetCertValue fail";
    return false;
  }

  const auto& root_cert = cert_info.author_root_certificate.get();
  if (pkgmgr_installer_set_cert_value(handle, PM_SET_AUTHOR_ROOT_CERT,
      const_cast<char*>(root_cert->getBase64().c_str())) < 0) {
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

int PkgmgrForeachAppCallback(const pkgmgrinfo_appinfo_h handle,
                              void *user_data) {
  auto* data = static_cast<std::vector<std::string>*>(user_data);
  char* app_id = nullptr;
  if (pkgmgrinfo_appinfo_get_appid(handle, &app_id) != PMINFO_R_OK)
    return PMINFO_R_ERROR;
  data->emplace_back(app_id);
  return PMINFO_R_OK;
}

int PkgmgrForeachPrivilegeCallback(const char* privilege_name,
                                   void* user_data) {
  auto* data = static_cast<std::vector<std::string>*>(user_data);
  data->emplace_back(privilege_name);
  return PMINFO_R_OK;
}

// "preload" : this package was installed at the binary creation.
// "system" : this package is "preload" and is not removable.
// "update" : this package is "preload" but is updated after first installation.
// "removable" : this package can be removed.
// "readonly" : this package exists in readonly location.
bool AssignPackageTags(manifest_x* manifest,
                       common_installer::RequestMode,
                       bool is_update) {
  if (!strcmp(manifest->preload, "true")) {
    manifest->removable = strdup("false");
    manifest->readonly = strdup("true");
    manifest->system = strdup("true");
    if (is_update)
      manifest->update = strdup("true");
    else
      manifest->update = strdup("false");
  } else {
    manifest->removable = strdup("true");
    manifest->readonly = strdup("false");
    manifest->system = strdup("false");
    manifest->update = strdup("false");
  }
  // external installation should alter this flag
  manifest->installed_storage = strdup("installed_internal");

  return true;
}

}  // anonymous namespace

namespace common_installer {

bool RegisterAppInPkgmgr(manifest_x* manifest,
                         const bf::path& xml_path,
                         const std::string& pkgid,
                         const CertificateInfo& cert_info,
                         uid_t uid,
                         RequestMode request_mode,
                         const boost::filesystem::path& tep_path) {
  // Fill "non-xml" elements
  if (!tep_path.empty())
    manifest->tep_name = strdup(tep_path.c_str());

  if (!AssignPackageTags(manifest, request_mode, false))
    return false;

  int ret = request_mode != RequestMode::GLOBAL ?
      pkgmgr_parser_process_usr_manifest_x_for_installation(manifest,
          xml_path.c_str(), uid) :
      pkgmgr_parser_process_manifest_x_for_installation(manifest,
          xml_path.c_str());
  if (ret) {
    LOG(ERROR) << "Failed to insert manifest into pkgmgr, error code=" << ret;
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

bool UpdateTepInfoInPkgmgr(const bf::path& tep_path, const std::string& pkgid,
                        uid_t uid, RequestMode request_mode) {
  int ret = request_mode != RequestMode::GLOBAL ?
        pkgmgr_parser_usr_update_tep(
            pkgid.c_str(), tep_path.string().c_str(), uid) :
        pkgmgr_parser_update_tep(
            pkgid.c_str(), tep_path.string().c_str());

  if (ret != 0) {
    LOG(ERROR) << "Failed to upgrade tep info: " << pkgid;
    return false;
  }

  return true;
}

bool UpgradeAppInPkgmgr(manifest_x* manifest,
                        const bf::path& xml_path,
                        const std::string& pkgid,
                        const CertificateInfo& cert_info,
                        uid_t uid,
                        RequestMode request_mode) {
  if (!AssignPackageTags(manifest, request_mode, true))
    return false;

  int ret = request_mode != RequestMode::GLOBAL ?
       pkgmgr_parser_process_usr_manifest_x_for_upgrade(manifest,
          xml_path.c_str(), uid) :
       pkgmgr_parser_process_manifest_x_for_upgrade(manifest, xml_path.c_str());

  if (ret != 0) {
    LOG(ERROR) << "Failed to update manifest in pkgmgr, error code=" << ret;
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

bool UnregisterAppInPkgmgr(manifest_x* manifest,
                           const bf::path& xml_path,
                           const std::string& pkgid,
                           uid_t uid,
                           RequestMode request_mode) {
  int ret = request_mode != RequestMode::GLOBAL ?
      pkgmgr_parser_process_usr_manifest_x_for_uninstallation(manifest,
          xml_path.c_str(), uid) :
      pkgmgr_parser_process_manifest_x_for_uninstallation(manifest,
          xml_path.c_str());
  if (ret) {
    LOG(ERROR) << "Failed to delete manifest from pkgmgr, error code=" << ret;
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
    LOG(DEBUG) << "pkgmgrinfo_pkginfo_create_certinfo failed with error: "
               << ret;
    return {};
  }
  ret = pkgmgrinfo_pkginfo_load_certinfo(pkgid.c_str(), handle, uid);
  if (ret != PMINFO_R_OK) {
    LOG(DEBUG) << "pkgmgrinfo_pkginfo_load_certinfo failed with error: " << ret;
    pkgmgrinfo_pkginfo_destroy_certinfo(handle);
    return {};
  }
  const char* author_cert = nullptr;
  ret = pkgmgrinfo_pkginfo_get_cert_value(handle, PMINFO_AUTHOR_SIGNER_CERT,
                                          &author_cert);
  if (ret != PMINFO_R_OK) {
    LOG(DEBUG) << "pkgmgrinfo_pkginfo_get_cert_value failed with error: "
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


bool QueryAppidsForPkgId(const std::string& pkg_id,
                         std::vector<std::string>* result, uid_t uid) {
  pkgmgrinfo_pkginfo_h package_info;
  if (pkgmgrinfo_pkginfo_get_usr_pkginfo(pkg_id.c_str(), uid, &package_info)
      != PMINFO_R_OK) {
    return false;
  }

  bool ret = pkgmgrinfo_appinfo_get_usr_list(package_info, PMINFO_ALL_APP,
      &PkgmgrForeachAppCallback, result, uid) == PMINFO_R_OK;
  pkgmgrinfo_pkginfo_destroy_pkginfo(package_info);
  return ret;
}

bool QueryPrivilegesForPkgId(const std::string& pkg_id, uid_t uid,
                             std::vector<std::string>* result) {
  pkgmgrinfo_pkginfo_h package_info;
  if (pkgmgrinfo_pkginfo_get_usr_pkginfo(pkg_id.c_str(), uid, &package_info)
      != PMINFO_R_OK) {
    return false;
  }

  bool ret = pkgmgrinfo_pkginfo_foreach_privilege(package_info,
      &PkgmgrForeachPrivilegeCallback, result) == PMINFO_R_OK;
  pkgmgrinfo_pkginfo_destroy_pkginfo(package_info);
  return ret;
}

bool IsPackageInstalled(const std::string& pkg_id, RequestMode request_mode) {
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
