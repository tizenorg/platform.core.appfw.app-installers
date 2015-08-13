// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_update_app.h"

#include <pkgmgr-info.h>
#include <sys/types.h>
#include <unistd.h>

#include <boost/filesystem.hpp>
#include <vcore/Certificate.h>

#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>

#include "common/pkgmgr_registration.h"
#include "common/utils/file_util.h"

namespace bf = boost::filesystem;

namespace common_installer {
namespace pkgmgr {

Step::Status StepUpdateApplication::precheck() {
  if (context_->xml_path.get().empty()) {
    LOG(ERROR) << "Xml path is empty";
    return Status::ERROR;
  }
  return Status::OK;
}

Step::Status StepUpdateApplication::process() {
  if (!UpgradeAppInPkgmgr(context_->xml_path.get(),
                          context_->pkgid.get(),
                          context_->certificate_info.get(),
                          context_->uid.get(),
                          context_->request_mode.get())) {
    LOG(ERROR) << "Cannot upgrade manifest for application";
    return Status::ERROR;
  }

  LOG(INFO) << "Successfully install the application";
  return Status::OK;
}

Step::Status StepUpdateApplication::undo() {
  // Prepare certification info for revert
  std::string base64 = QueryCertificateAuthorCertificate(context_->pkgid.get(),
                                                         context_->uid.get());
  CertificateInfo certificate_info;
  if (!base64.empty()) {
    try {
      certificate_info.author_certificate.set(ValidationCore::CertificatePtr(
          new ValidationCore::Certificate(
              base64,
              ValidationCore::Certificate::FormType::FORM_BASE64)));
    } catch (const ValidationCore::Certificate::Exception::Base &e) {
      LOG(ERROR) << "Exception in cert-svc-vcore Certificate "
                 << "Dump : " << e.DumpToString();
      return Status::ERROR;
    }
  }

  if (!UpgradeAppInPkgmgr(context_->backup_xml_path.get(),
                          context_->pkgid.get(), certificate_info,
                          context_->uid.get(),
                          context_->request_mode.get())) {
    LOG(ERROR) << "Cannot revert manifest for application";
    return Status::ERROR;
  }
  LOG(INFO) << "Database reverted successfully";
  return Status::OK;
}

}  // namespace pkgmgr
}  // namespace common_installer
