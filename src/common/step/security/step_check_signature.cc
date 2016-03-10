// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/security/step_check_signature.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/format.hpp>
#include <glib.h>
#include <privilege_manager.h>

#include <vcore/Certificate.h>
#include <vcore/SignatureFinder.h>
#include <vcore/SignatureValidator.h>
#include <vcore/Error.h>

#include <cassert>
#include <cstdlib>
#include <string>

#include "common/utils/base64.h"
#include "common/utils/glist_range.h"
#include "common/pkgmgr_registration.h"

namespace bf = boost::filesystem;
namespace ci = common_installer;

namespace {

bool CheckPkgCertificateMismatch(const std::string& pkgid,
                                 const std::string& old_certificate) {
  bool certificate_mismatch = false;
  uid_t uid = G_MAXUINT;
  auto certificate = ci::QueryCertificateAuthorCertificate(pkgid, uid);

  if (!certificate.empty()) {
    certificate_mismatch = (old_certificate != certificate);
  }
  return certificate_mismatch;
}

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

void SetPrivilegeLevel(ValidationCore::SignatureData data,
    common_installer::PrivilegeLevel* level) {
  // already set
  if (*level != common_installer::PrivilegeLevel::UNTRUSTED)
    return;
  *level = CertStoreIdToPrivilegeLevel(data.getVisibilityLevel());
}

void SetAuthorCertificate(ValidationCore::SignatureData data,
    common_installer::CertificateInfo* cert_info) {
  ValidationCore::CertificateList cert_list = data.getCertList();
  ValidationCore::CertificateList::iterator it = cert_list.begin();
  unsigned char* public_key;
  size_t len;
  (*it)->getPublicKeyDER(&public_key, &len);
  std::string author_id =
      ci::EncodeBase64(reinterpret_cast<const char*>(public_key));
  cert_info->author_id.set(author_id);
  cert_info->author_certificate.set(*it);
  // cert_list has at least 3 certificates: end-user, intermediate, root
  // currently pkgmgr can store only one intermediate cert, so just set
  // first intermediate cert here.
  ++it;
  cert_info->author_intermediate_certificate.set(*it);
  cert_info->author_root_certificate.set(data.getRootCaCertificatePtr());
}

void SetDistributorCertificate(ValidationCore::SignatureData data,
    common_installer::CertificateInfo* cert_info) {
  ValidationCore::CertificateList cert_list = data.getCertList();
  ValidationCore::CertificateList::iterator it = cert_list.begin();
  cert_info->distributor_certificate.set(*it);
  ++it;
  cert_info->distributor_intermediate_certificate.set(*it);
  cert_info->distributor_root_certificate.set(data.getRootCaCertificatePtr());
}

common_installer::Step::Status ValidateSignatureFile(
    const bf::path& base_path,
    const ValidationCore::SignatureFileInfo& file_info,
    common_installer::PrivilegeLevel* level,
    common_installer::CertificateInfo* cert_info,
    bool check_reference, std::string* error_message) {
  bf::path path = base_path / file_info.getFileName();
  LOG(INFO) << "Processing signature: " << path;

  ValidationCore::SignatureValidator validator(file_info);
  ValidationCore::SignatureData data;
  ValidationCore::VCerr result = validator.check(
      base_path.string(),  // app content path for checking hash of file ref.
      true,                // ocsp check flag
      check_reference,     // file reference hash check flag
      data);               // output signature data

  std::string errnum =
                  boost::str(boost::format("%d") % result);
  *error_message = validator.errorToString(result);
  *error_message += ":<" + errnum + ">";

  switch (result) {
    case ValidationCore::E_SIG_REVOKED:
      LOG(ERROR) << "Certificate is revoked";
      return common_installer::Step::Status::SIGNATURE_INVALID;
    case ValidationCore::E_SIG_DISREGARDED:
        if (data.isAuthorSignature()) {
          LOG(ERROR) << "Author-signiture is disregarded";
          return common_installer::Step::Status::SIGNATURE_INVALID;
        }
        LOG(WARNING) << "Signature disregarded: " << path;
        break;
    case ValidationCore::E_SIG_NONE:
      if (data.isAuthorSignature()) {
        // set author certificates to be saved in pkgmgr
        SetAuthorCertificate(data, cert_info);
      } else if (file_info.getFileNumber() == 1) {
        // First distributor signature sets the privilege level
        // (wrt spec. 0620.)
        SetPrivilegeLevel(data, level);
        SetDistributorCertificate(data, cert_info);
      }
      // TODO(s89.jang): Set distributor2 certificate
      break;
    default:
      LOG(ERROR) << "signature validation check failed : "
                 << validator.errorToString(result);
      return common_installer::Step::Status::SIGNATURE_INVALID;
  }
  return common_installer::Step::Status::OK;
}

bool ValidatePrivilegeLevel(common_installer::PrivilegeLevel level,
    bool is_webapp, const char* api_version, GList* privileges,
    std::string* error_message) {
  if (level == common_installer::PrivilegeLevel::UNTRUSTED) {
    if (privileges) {
      LOG(ERROR) << "Untrusted application cannot declare privileges";
      return false;
    } else {
      return true;
    }
  }

  char* error = nullptr;
  int status = PRVMGR_ERR_NONE;
  // Do the privilege check only if the package has privileges
  if (privileges) {
    status = privilege_manager_verify_privilege(api_version,
        is_webapp ? PRVMGR_PACKAGE_TYPE_WRT : PRVMGR_PACKAGE_TYPE_CORE,
        privileges, PrivilegeLevelToVisibility(level), &error);
  }
  if (status != PRVMGR_ERR_NONE) {
    std::string errnum =
                   boost::str(boost::format("%d") % status);
    LOG(ERROR) << "Error while verifing privilege level: "
               << error << " <" << errnum << ">";
    *error_message = error;
    *error_message += ":<" + errnum + ">";
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
    PrivilegeLevel* level, common_installer::CertificateInfo* cert_info,
    bool check_reference, std::string* error_message) {
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
    std::string error;
    Step::Status status = ValidateSignatureFile(base_path, file_info, level,
                                        cert_info, check_reference, &error);
    if (status != Step::Status::OK) {
      *error_message = error;
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
  std::string error_message;
  bool check_reference = true;
  if (context_->uid.get() == 0 &&
      (context_->request_type.get()== ci::RequestType::ManifestDirectInstall ||
      context_->request_type.get() == ci::RequestType::ManifestDirectUpdate))
    check_reference = false;
  Status status =
      ValidateSignatures(context_->unpacked_dir_path.get(), &level,
                         &context_->certificate_info.get(), check_reference,
                         &error_message);
  if (status != Status::OK) {
    on_error(status, error_message);
    return status;
  }

  const auto& cert = context_->certificate_info.get().author_certificate.get();
  if (cert) {
    bool certificate_mismatch =
        CheckPkgCertificateMismatch(context_->pkgid.get(), cert->getBase64());
    if (certificate_mismatch) {
      std::string error_message =
          "Package with the same id and different certificate "
          "has been already installed";
      on_error(Status::CERT_ERROR, error_message);
      return Status::CERT_ERROR;
    }
  }

  if (context_->is_preload_request.get())
    level = PrivilegeLevel::PLATFORM;

  LOG(INFO) << "Privilege level: " << PrivilegeLevelToString(level);
  context_->privilege_level.set(level);

  // TODO(t.iwanek): refactoring, move to wgt backend
  bool is_webapp = context_->pkg_type.get() == "wgt";
  error_message.clear();
  if (!context_->is_preload_request.get()) {
    if (!ValidatePrivilegeLevel(level, is_webapp,
        context_->manifest_data.get()->api_version,
        context_->manifest_data.get()->privileges, &error_message)) {
      if (!error_message.empty()) {
        LOG(ERROR) << "error_message: " << error_message;
        on_error(Status::SIGNATURE_ERROR, error_message);
      }
      return Status::SIGNATURE_ERROR;
    }
  }

  LOG(INFO) << "Signature done";
  return Status::OK;
}

}  // namespace security
}  // namespace common_installer
