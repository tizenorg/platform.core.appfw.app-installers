// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_check_old_certificate.h"

#include <gum/gum-user.h>
#include <gum/common/gum-user-types.h>
#include <gum/gum-user-service.h>
#include <gum/common/gum-string-utils.h>

#include <manifest_parser/utils/logging.h>
#include <pkgmgr-info.h>
#include <unistd.h>

#include <cstdlib>
#include <string>

#include "common/pkgmgr_registration.h"
#include "common/utils/glist_range.h"

namespace common_installer {
namespace security {

bool CheckPkgCertificateMismatch(const std::string& pkgid,
                                 const uid_t current_uid,
                                 const std::string& old_certificate) {
  gchar **users_types = NULL;
  users_types = gum_string_utils_append_string(nullptr, "normal");
  users_types = gum_string_utils_append_string(users_types, "admin");
  users_types = gum_string_utils_append_string(users_types, "guest");

  GumUserService *user_service = NULL;
  user_service = gum_user_service_create_sync(FALSE);

  GumUserList *users = NULL;
  users = gum_user_service_get_user_list_sync(user_service,
                                             (const gchar *const *)users_types);
  g_strfreev(users_types);

  bool certificate_mismatch = false;

  if (users) {
    uid_t uid = G_MAXUINT;

    for (GumUser* guser : GListRange<GumUser*>(users)) {
      g_object_get(G_OBJECT(guser), "uid", &uid, NULL);
      if (current_uid == uid) continue;

      auto certificate = QueryCertificateAuthorCertificate(pkgid, uid);
      if (!certificate.empty()) {
        certificate_mismatch = (old_certificate != certificate);
        if (certificate_mismatch)
          LOG(DEBUG) << "Found app with mismatched certificate: " << pkgid;
      }
    }
    gum_user_service_list_free(users);
  }

  g_object_unref(user_service);
  return certificate_mismatch;
}


Step::Status StepCheckOldCertificate::process() {
  std::string old_author_certificate =
      QueryCertificateAuthorCertificate(context_->pkgid.get(),
                                        context_->uid.get());
  const auto& cert = context_->certificate_info.get().author_certificate.get();

  if (cert) {
    bool certificate_mismatch =
        CheckPkgCertificateMismatch(context_->pkgid.get(),
                                    context_->uid.get(),
                                    cert->getBase64());
    if (certificate_mismatch) {
      LOG(ERROR) << "Package with the same id and different certificate "
                 << "has been already installed";
      return Status::CERT_ERROR;
    }
  }

  if (!old_author_certificate.empty()) {
    if (!cert) {
      LOG(ERROR) << "Trying to update package without signature is not allowed "
                 << "when the previous version of package has signature";
      return Status::AUTHOR_CERT_NOT_FOUND;
    } else {
      try {
        if (old_author_certificate != cert->getBase64()) {
          LOG(ERROR) << "Author signature doesn't match the previous one. "
                     << "Update must be aborted";
          return Status::AUTHOR_CERT_NOT_MATCH;
        }
      } catch (const ValidationCore::Certificate::Exception::Base &e) {
        LOG(ERROR) << "Exception occured on cert-svc-vcore getBase64 "
                   << "Dump : " << e.DumpToString();
        return Status::CERT_ERROR;
      }
    }
  } else {
    if (cert) {
      LOG(ERROR) << "Trying to update package with signature is not allowed "
                 << "when the previous version of package has not signature";
      return Status::AUTHOR_CERT_NOT_MATCH;
    }
  }

  return Status::OK;
}

}  // namespace security
}  // namespace common_installer
