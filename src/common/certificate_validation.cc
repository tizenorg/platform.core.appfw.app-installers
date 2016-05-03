// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/certificate_validation.h"

#include <algorithm>
#include <regex>

#include <boost/format.hpp>
#include <vcore/SignatureValidator.h>

#include "common/utils/base64.h"

namespace bf = boost::filesystem;
namespace ci = common_installer;

namespace {

const char kSignatureAuthor[] = "author-signature.xml";
const char kRegexDistributorSignature[] = "^(signature)([1-9][0-9]*)(\\.xml)";


bool SetAuthorCertificate(ValidationCore::SignatureData data,
    common_installer::CertificateInfo* cert_info) {
  ValidationCore::CertificateList cert_list = data.getCertList();
  ValidationCore::CertificateList::iterator it = cert_list.begin();
  if (it == cert_list.end()) {
    LOG(ERROR) << "No certificates in certificate list";
    return false;
  }
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
  if (it == cert_list.end()) {
    LOG(ERROR) << "No intermediate certificates in certificate list";
    return false;
  }
  cert_info->author_intermediate_certificate.set(*it);
  cert_info->author_root_certificate.set(data.getRootCaCertificatePtr());
  return true;
}

bool SetDistributorCertificate(ValidationCore::SignatureData data,
    common_installer::CertificateInfo* cert_info) {
  ValidationCore::CertificateList cert_list = data.getCertList();
  ValidationCore::CertificateList::iterator it = cert_list.begin();
  if (it == cert_list.end()) {
    LOG(ERROR) << "No certificates in certificate list";
    return false;
  }
  cert_info->distributor_certificate.set(*it);
  ++it;
  if (it == cert_list.end()) {
    LOG(ERROR) << "No intermediate certificates in certificate list";
    return false;
  }
  cert_info->distributor_intermediate_certificate.set(*it);
  cert_info->distributor_root_certificate.set(data.getRootCaCertificatePtr());
  return true;
}

}  // namespace

namespace common_installer {

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

bool ValidateSignatureFile(
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

  std::string errnum = boost::str(boost::format("%d") % result);
  *error_message = validator.errorToString(result);
  *error_message += ":<" + errnum + ">";

  switch (result) {
    case ValidationCore::E_SIG_REVOKED:
      LOG(ERROR) << "Certificate is revoked";
      return false;
    case ValidationCore::E_SIG_DISREGARDED:
    case ValidationCore::E_SIG_NONE:
      if (data.isAuthorSignature()) {
        // set author certificates to be saved in pkgmgr
        if (!SetAuthorCertificate(data, cert_info))
          return false;
      } else if (file_info.getFileNumber() == 1) {
        // First distributor signature sets the privilege level
        // (wrt spec. 0620.)
        SetPrivilegeLevel(data, level);
        if (!SetDistributorCertificate(data, cert_info))
          return false;
      }
      // TODO(s89.jang): Set distributor2 certificate
      break;
    default:
      LOG(ERROR) << "signature validation check failed : "
                 << validator.errorToString(result);
      return false;
  }
  return true;
}

bool CheckAuthorSignature(const ValidationCore::SignatureFileInfo& file_info) {
  return file_info.getFileName().find(kSignatureAuthor) != std::string::npos;
}

bool CheckDistSignature(const ValidationCore::SignatureFileInfo& file_info) {
  std::regex distributor_regex(kRegexDistributorSignature);
  return std::regex_search(file_info.getFileName(), distributor_regex);
}

bool ValidateSignatures(const bf::path& base_path,
    PrivilegeLevel* level, common_installer::CertificateInfo* cert_info,
    bool check_reference, std::string* error_message) {
  // Find signature files
  ValidationCore::SignatureFileInfoSet signature_files;
  ValidationCore::SignatureFinder signature_finder(base_path.string());
  if (signature_finder.find(signature_files) !=
      ValidationCore::SignatureFinder::NO_ERROR) {
    LOG(ERROR) << "Error while searching for signatures";
    return false;
  }
  LOG(INFO) << "Number of signature files: " << signature_files.size();

  bool author_signatures = std::any_of(
      signature_files.begin(), signature_files.end(), CheckAuthorSignature);

  bool distributor_signatures = std::any_of(
      signature_files.begin(), signature_files.end(), CheckDistSignature);

  if (!author_signatures || !distributor_signatures) {
    LOG(ERROR) << "Author or distribuor signature is missing.";
    return false;
  }

  // Read xml schema for signatures
  for (auto& file_info : signature_files) {
    std::string error;
    if (!ValidateSignatureFile(base_path, file_info, level,
                               cert_info, check_reference, &error)) {
      *error_message = error;
      return false;
    }
  }
  return true;
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

}  // namespace common_installer
