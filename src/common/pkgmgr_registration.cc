// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/pkgmgr_registration.h"

#include <manifest_parser/utils/logging.h>
#include <pkgmgr_installer.h>
#include <pkgmgr_parser_db.h>
#include <tzplatform_config.h>
#include <unistd.h>

#include <vector>

namespace bf = boost::filesystem;

namespace {

bool RegisterCertificates(
    const common_installer::CertificateInfo& cert_info,
    const std::string& pkgid, uid_t uid) {
  pkgmgr_instcertinfo_h handle;
  if (pkgmgr_installer_create_certinfo_set_handle(&handle) < 0) {
    LOG(ERROR) << "Cannot create pkgmgr_instcertinfo_h";
    return false;
  }

  const auto& author_cert = cert_info.author_certificate.get();
  if (author_cert) {
    if (pkgmgr_installer_set_cert_value(handle, PM_SET_AUTHOR_SIGNER_CERT,
        const_cast<char*>(author_cert->getBase64().c_str())) < 0) {
      pkgmgr_installer_destroy_certinfo_set_handle(handle);
      LOG(ERROR) << "pkgmgrInstallerSetCertValue fail";
      return false;
    }
  }

  const auto& author_im_cert = cert_info.author_intermediate_certificate.get();
  if (author_im_cert) {
    if (pkgmgr_installer_set_cert_value(handle, PM_SET_AUTHOR_INTERMEDIATE_CERT,
        const_cast<char*>(author_im_cert->getBase64().c_str())) < 0) {
      pkgmgr_installer_destroy_certinfo_set_handle(handle);
      LOG(ERROR) << "pkgmgrInstallerSetCertValue fail";
      return false;
    }
  }

  const auto& author_root_cert = cert_info.author_root_certificate.get();
  if (author_root_cert) {
    if (pkgmgr_installer_set_cert_value(handle, PM_SET_AUTHOR_ROOT_CERT,
        const_cast<char*>(author_root_cert->getBase64().c_str())) < 0) {
      pkgmgr_installer_destroy_certinfo_set_handle(handle);
      LOG(ERROR) << "pkgmgrInstallerSetCertValue fail";
      return false;
    }
  }

  const auto& dist_cert = cert_info.distributor_certificate.get();
  if (dist_cert) {
    if (pkgmgr_installer_set_cert_value(handle, PM_SET_DISTRIBUTOR_SIGNER_CERT,
        const_cast<char*>(dist_cert->getBase64().c_str())) < 0) {
      pkgmgr_installer_destroy_certinfo_set_handle(handle);
      LOG(ERROR) << "pkgmgrInstallerSetCertValue fail";
      return false;
    }
  }

  const auto& dist_im_cert =
      cert_info.distributor_intermediate_certificate.get();
  if (dist_im_cert) {
    if (pkgmgr_installer_set_cert_value(handle,
        PM_SET_DISTRIBUTOR_INTERMEDIATE_CERT,
        const_cast<char*>(dist_im_cert->getBase64().c_str())) < 0) {
      pkgmgr_installer_destroy_certinfo_set_handle(handle);
      LOG(ERROR) << "pkgmgrInstallerSetCertValue fail";
      return false;
    }
  }

  const auto& dist_root_cert = cert_info.distributor_root_certificate.get();
  if (dist_root_cert) {
    if (pkgmgr_installer_set_cert_value(handle, PM_SET_DISTRIBUTOR_ROOT_CERT,
        const_cast<char*>(dist_root_cert->getBase64().c_str())) < 0) {
      pkgmgr_installer_destroy_certinfo_set_handle(handle);
      LOG(ERROR) << "pkgmgrInstallerSetCertValue fail";
      return false;
    }
  }

  const auto& dist2_cert = cert_info.distributor2_certificate.get();
  if (dist2_cert) {
    if (pkgmgr_installer_set_cert_value(handle, PM_SET_DISTRIBUTOR2_SIGNER_CERT,
        const_cast<char*>(dist2_cert->getBase64().c_str())) < 0) {
      pkgmgr_installer_destroy_certinfo_set_handle(handle);
      LOG(ERROR) << "pkgmgrInstallerSetCertValue fail";
      return false;
    }
  }

  const auto& dist2_im_cert =
      cert_info.distributor2_intermediate_certificate.get();
  if (dist2_im_cert) {
    if (pkgmgr_installer_set_cert_value(handle,
        PM_SET_DISTRIBUTOR2_INTERMEDIATE_CERT,
        const_cast<char*>(dist2_im_cert->getBase64().c_str())) < 0) {
      pkgmgr_installer_destroy_certinfo_set_handle(handle);
      LOG(ERROR) << "pkgmgrInstallerSetCertValue fail";
      return false;
    }
  }

  const auto& dist2_root_cert = cert_info.distributor2_root_certificate.get();
  if (dist2_root_cert) {
    if (pkgmgr_installer_set_cert_value(handle, PM_SET_DISTRIBUTOR2_ROOT_CERT,
        const_cast<char*>(dist2_root_cert->getBase64().c_str())) < 0) {
      pkgmgr_installer_destroy_certinfo_set_handle(handle);
      LOG(ERROR) << "pkgmgrInstallerSetCertValue fail";
      return false;
    }
  }

  if (pkgmgr_installer_save_certinfo(pkgid.c_str(), handle, uid) < 0) {
    pkgmgr_installer_destroy_certinfo_set_handle(handle);
    LOG(ERROR) << "Failed to save certificate information";
    return false;
  }

  pkgmgr_installer_destroy_certinfo_set_handle(handle);
  return true;
}

// "preload" : this package was installed at the binary creation.
// "system" : this package is "preload" and is not removable.
// "update" : this package is "preload" but is updated after first installation.
// "removable" : this package can be removed.
// "readonly" : this package exists in readonly location.
bool AssignPackageTags(manifest_x* manifest,
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

  if (!AssignPackageTags(manifest, false))
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

  if (!RegisterCertificates(cert_info, pkgid, uid)) {
    LOG(ERROR) << "Failed to register author certificate";
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
  if (!AssignPackageTags(manifest, true))
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
  if (!RegisterCertificates(cert_info, pkgid, uid))
    return false;

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

bool DisablePkgInPkgmgr(const std::string& pkgid, uid_t uid,
                        RequestMode request_mode) {
  int ret = request_mode != RequestMode::GLOBAL ?
        pkgmgr_parser_update_pkg_disable_info_in_usr_db(pkgid.c_str(), uid, 1) :
        pkgmgr_parser_update_pkg_disable_info_in_db(pkgid.c_str(), 1);
  if (ret != 0) {
    LOG(ERROR) << "Failed to disable pkg: " << pkgid;
    return false;
  }

  return true;
}

bool EnablePkgInPkgmgr(const std::string& pkgid, uid_t uid,
                        RequestMode request_mode) {
  int ret = request_mode != RequestMode::GLOBAL ?
        pkgmgr_parser_update_pkg_disable_info_in_usr_db(pkgid.c_str(), uid, 0) :
        pkgmgr_parser_update_pkg_disable_info_in_db(pkgid.c_str(), 0);
  if (ret != 0) {
    LOG(ERROR) << "Failed to enable pkg: " << pkgid;
    return false;
  }

  return true;
}

}  // namespace common_installer
