// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/security/step_check_signature.h"

#include <boost/filesystem/operations.hpp>
#include <glib.h>

#include <cassert>
#include <cstdlib>
#include <string>

#include "common/certificate_validation.h"
#include "common/pkgmgr_query.h"
#include "common/utils/glist_range.h"

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

}  // namespace

namespace common_installer {
namespace security {

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

boost::filesystem::path StepCheckSignature::GetSignatureRoot() const {
  return context_->unpacked_dir_path.get();
}

Step::Status StepCheckSignature::CheckSignatures(bool check_reference,
                                                 bool is_preload,
                                                 PrivilegeLevel* level) {
  std::string error_message;
  if (!ValidateSignatures(GetSignatureRoot(), level,
                         &context_->certificate_info.get(), check_reference,
                         is_preload, &error_message)) {
    on_error(Status::CERT_ERROR, error_message);
    return Status::CERT_ERROR;
  }
  return Status::OK;
}

Step::Status StepCheckSignature::CheckSignatureMismatch() {
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
  return Status::OK;
}

Step::Status StepCheckSignature::CheckPrivilegeLevel(PrivilegeLevel level) {
  // TODO(t.iwanek): refactoring, move to wgt backend
  bool is_webapp = context_->pkg_type.get() == "wgt";
  std::string error_message;
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
  return Status::OK;
}

Step::Status StepCheckSignature::process() {
  PrivilegeLevel level = PrivilegeLevel::UNTRUSTED;
  bool check_reference = true;
  if (context_->uid.get() == 0 &&
      (context_->request_type.get() == ci::RequestType::ManifestDirectInstall ||
      context_->request_type.get() == ci::RequestType::ManifestDirectUpdate))
    check_reference = false;
  bool is_preload = context_->is_preload_request.get();
  Status status = CheckSignatures(check_reference, is_preload, &level);
  if (status != Status::OK)
    return status;

  status = CheckSignatureMismatch();
  if (status != Status::OK)
    return status;

  if (is_preload)
    level = PrivilegeLevel::PLATFORM;

  if (level == PrivilegeLevel::UNTRUSTED) {
    std::string error_message =
        "Unsigned applications can not be installed";
    on_error(Status::CERT_ERROR, error_message);
    return Status::SIGNATURE_ERROR;
  }

  LOG(INFO) << "Privilege level: " << PrivilegeLevelToString(level);
  context_->privilege_level.set(level);

  status = CheckPrivilegeLevel(level);
  if (status != Status::OK)
    return status;

  LOG(INFO) << "Signature done";
  return Status::OK;
}

}  // namespace security
}  // namespace common_installer
