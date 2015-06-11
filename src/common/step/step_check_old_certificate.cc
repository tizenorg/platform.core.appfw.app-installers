// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_check_old_certificate.h"

#include <pkgmgr-info.h>
#include <unistd.h>

#include <cstdlib>
#include <string>

#include "common/pkgmgr_registration.h"
#include "common/utils/logging.h"

namespace common_installer {
namespace old_certificate {

Step::Status StepCheckOldCertificate::process() {
  std::string old_author_certificate =
      QueryCertificateAuthorCertificate(context_->pkgid.get(),
                                        context_->uid.get());
  const auto& cert = context_->certificate_info.get().author_certificate.get();
  if (!old_author_certificate.empty()) {
    if (!cert) {
      LOG(ERROR) << "Trying to update package without signature is not allowed "
                 << "when the previous version of package has signature";
      return Status::ERROR;
    } else {
      if (old_author_certificate != cert->getBase64()) {
        LOG(ERROR) << "Author signature doesn't match the previous one. "
                   << "Update must be aborted";
        return Status::ERROR;
      }
    }
  } else {
    if (cert) {
      LOG(ERROR) << "Trying to update package with signature is not allowed "
                 << "when the previous version of package has not signature";
      return Status::ERROR;
    }
  }

  return Status::OK;
}

}  // namespace old_certificate
}  // namespace common_installer