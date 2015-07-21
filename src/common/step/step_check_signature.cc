// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_check_signature.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <glib.h>
#include <privilege_manager.h>

#include <vcore/SignatureFinder.h>
#include <vcore/SignatureValidator.h>

#include <cassert>
#include <cstdlib>
#include <string>

#include "common/utils/clist_helpers.h"

namespace bf = boost::filesystem;

namespace {

common_installer::PrivilegeLevel CertStoreIdToPrivilegeLevel(
    ValidationCore::CertStoreId::Type id) {
  switch (id) {
    case ValidationCore::CertStoreId::VIS_PUBLIC:
      return common_installer::PrivilegeLevel::PUBLIC;
    case ValidationCore::CertStoreId::VIS_PARTNER:
      return common_installer::PrivilegeLevel::PARTNER;
    case ValidationCore::CertStoreId::VIS_PLATFORM:
      return common_installer::PrivilegeLevel::PLATFORM;
    default:
      return common_installer::PrivilegeLevel::UNTRUSTED;
  }
}

privilege_manager_visibility_e PrivilegeLevelToVisibility(
    common_installer::PrivilegeLevel level) {
  switch (level) {
    case common_installer::PrivilegeLevel::PUBLIC:
      return PRVMGR_PACKAGE_VISIBILITY_PUBLIC;
    case common_installer::PrivilegeLevel::PARTNER:
      return PRVMGR_PACKAGE_VISIBILITY_PARTNER;
    case common_installer::PrivilegeLevel::PLATFORM:
      return PRVMGR_PACKAGE_VISIBILITY_PLATFORM;
    default:
      assert(false && "Not reached");
  }
}

common_installer::Step::Status ValidateSignatureFile(
    const bf::path& base_path,
    const ValidationCore::SignatureFileInfo& file_info,
    common_installer::PrivilegeLevel* level,
    common_installer::CertificateInfo* cert_info) {
  bf::path path = base_path / file_info.getFileName();
  LOG(INFO) << "Processing signature: " << path;

  ValidationCore::SignatureData data;
  ValidationCore::SignatureValidator::Result result =
    ValidationCore::SignatureValidator::check(
      file_info,           // signature file info
      base_path.string(),  // app content path for checking hash of file ref.
      true,                // ocsp check flag
      true,                // file reference hash check flag
      data);               // output signature data

  switch (result) {
    case ValidationCore::SignatureValidator::SIGNATURE_REVOKED: {
      LOG(ERROR) << "Certificate is revoked";
      return common_installer::Step::Status::ERROR;
    };
    case ValidationCore::SignatureValidator::SIGNATURE_INVALID: {
      LOG(ERROR) << "Certificate is invalid";
      return common_installer::Step::Status::ERROR;
    };
    case ValidationCore::SignatureValidator::SIGNATURE_DISREGARD: {
        if (data.isAuthorSignature()) {
          LOG(ERROR) << "Author-signiture is disregarded";
          return common_installer::Step::Status::ERROR;
        }
        LOG(WARNING) << "Signature disregarded: " << path;
        break;
    };
    case ValidationCore::SignatureValidator::SIGNATURE_VERIFIED: {
      if (!data.isAuthorSignature()) {
        // First distributor signature sets the privilege level
        // (wrt spec. 0620.)
        if (file_info.getFileNumber() == 1 &&
            *level == common_installer::PrivilegeLevel::UNTRUSTED) {
          *level = CertStoreIdToPrivilegeLevel(data.getVisibilityLevel());
        }
      } else {
        // set author certificate to be saved in pkgmgr
        cert_info->author_certificate.set(data.getEndEntityCertificatePtr());
      }
      break;
    };
    default: {
      return common_installer::Step::Status::ERROR;
    };
  }
  return common_installer::Step::Status::OK;
}

bool ValidatePrivilegeLevel(common_installer::PrivilegeLevel level,
    bool is_webapp, const char* api_version, privileges_x *privileges) {
  GList* list = nullptr;
  privileges_x* pvlg = nullptr;
  PKGMGR_LIST_MOVE_NODE_TO_HEAD(privileges, pvlg);
  for (; pvlg != nullptr; pvlg = pvlg->next) {
    privilege_x* pv = nullptr;
    PKGMGR_LIST_MOVE_NODE_TO_HEAD(pvlg->privilege, pv);
    for (; pv != nullptr; pv = pv->next) {
      list = g_list_append(list, const_cast<char*>(pv->text));
    }
  }

  if (level == common_installer::PrivilegeLevel::UNTRUSTED) {
    if (list) {
      g_list_free(list);
      LOG(ERROR) << "Untrusted application cannot declare privileges";
      return false;
    } else {
      return true;
    }
  }

  char* error = nullptr;
  int status = PRVMGR_ERR_NONE;
  if (list) {  // Do the privilege check only if the package has privileges
    status = privilege_manager_verify_privilege(api_version,
        is_webapp ? PRVMGR_PACKAGE_TYPE_WRT : PRVMGR_PACKAGE_TYPE_CORE, list,
        PrivilegeLevelToVisibility(level),
        &error);
    g_list_free(list);
  }
  if (status != PRVMGR_ERR_NONE) {
    LOG(ERROR) << "Error while verifing privilege level: " << error;
    free(error);
    return false;
  }
  LOG(INFO) << "Privilege level checked";
  return true;
}

}  // namespace

namespace common_installer {
namespace security {

Step::Status ValidateSignatures(const bf::path& base_path,
    PrivilegeLevel* level, common_installer::CertificateInfo* cert_info) {
  // Find signature files
  ValidationCore::SignatureFileInfoSet signature_files;
  ValidationCore::SignatureFinder signature_finder(base_path.string());
  if (signature_finder.find(signature_files) !=
      ValidationCore::SignatureFinder::NO_ERROR) {
    LOG(ERROR) << "Error while searching for signatures";
    return Step::Status::ERROR;
  }
  LOG(INFO) << "Number of signature files: " << signature_files.size();

  // Read xml schema for signatures
  for (auto& file_info : signature_files) {
    Step::Status status = ValidateSignatureFile(base_path, file_info, level,
                                                cert_info);
    if (status != Step::Status::OK) {
      return status;
    }
  }
  return Step::Status::OK;
}

Step::Status StepCheckSignature::precheck() {
  if (context_->unpacked_dir_path.get().empty()) {
    LOG(ERROR) << "unpacked_dir_path attribute is empty";
    return Step::Status::INVALID_VALUE;
  }
  if (!boost::filesystem::exists(context_->unpacked_dir_path.get())) {
    LOG(ERROR) << "unpacked_dir_path ("
               << context_->unpacked_dir_path.get()
               << ") path does not exist";
    return Step::Status::INVALID_VALUE;
  }

  return Step::Status::OK;
}

Step::Status StepCheckSignature::process() {
  PrivilegeLevel level = PrivilegeLevel::UNTRUSTED;
  Status status =
      ValidateSignatures(context_->unpacked_dir_path.get(), &level,
                         &context_->certificate_info.get());
  if (status != Status::OK) {
    return status;
  }

  LOG(INFO) << "Privilege level: " << PrivilegeLevelToString(level);
  context_->privilege_level.set(level);

  // TODO(t.iwanek): refactoring, move to wgt backend
  bool is_webapp = context_->pkg_type.get() == "wgt";
  if (!ValidatePrivilegeLevel(level, is_webapp,
      context_->config_data.get().required_api_version.get().c_str(),
      context_->manifest_data.get()->privileges))
    return Status::ERROR;

  LOG(INFO) << "Signature done";
  return Status::OK;
}

}  // namespace security
}  // namespace common_installer