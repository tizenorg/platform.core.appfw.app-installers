// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_check_old_certificate.h"

#include <pkgmgr-info.h>
#include <unistd.h>

#include <cstdlib>
#include <string>

#include "common/utils/logging.h"

namespace {

std::string QueryOldCertificateAuthorCertificate(const std::string& pkgid) {
  pkgmgrinfo_certinfo_h handle;
  int ret = pkgmgrinfo_pkginfo_create_certinfo(&handle);
  if (ret != PMINFO_R_OK) {
    LOG(ERROR) << "pkgmgrinfo_pkginfo_create_certinfo failed with error: "
               << ret;
    return {};
  }
  ret = pkgmgrinfo_pkginfo_load_certinfo(pkgid.c_str(), handle, getuid());
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

}  // namespace

namespace common_installer {
namespace old_certificate {

Step::Status StepCheckOldCertificate::process() {
  std::string old_author_certificate =
      QueryOldCertificateAuthorCertificate(context_->pkgid.get());
  if (old_author_certificate.empty())
    return Status::OK;

  const auto& cert = context_->certificate_info.get().author_certificate.get();
  if (!cert) {
    LOG(ERROR) << "Trying to update package without signature is not allowed "
               << "when the previous version of package has signature";
    return Status::ERROR;
  }
  if (old_author_certificate != cert->getBase64()) {
    LOG(ERROR) << "Author signature doesn't match the previous one. "
               << "Update must be aborted";
    return Status::ERROR;
  }

  return Status::OK;
}

}  // namespace old_certificate
}  // namespace common_installer
