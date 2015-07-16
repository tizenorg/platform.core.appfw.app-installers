// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include <unistd.h>

#include <boost/filesystem.hpp>
#include <pkgmgr_installer.h>
#include <vcore/Base64.h>
#include <vcore/Certificate.h>

#include <cassert>
#include <string>

#include "common/pkgmgr_registration.h"
#include "common/step/step_unregister_app.h"
#include "common/utils/file_util.h"

namespace common_installer {
namespace pkgmgr {

namespace fs = boost::filesystem;

Step::Status StepUnregisterApplication::precheck() {
  if (context_->pkgid.get().empty()) {
    LOG(ERROR) << "pkgid attribute is empty";
    return Status::INVALID_VALUE;
  }
  if (context_->xml_path.get().empty()) {
    LOG(ERROR) << "xml_path attribute is empty";
    return Status::INVALID_VALUE;
  }
  if (!boost::filesystem::exists(context_->xml_path.get())) {
    LOG(ERROR) << "xml_path ("
               << context_->xml_path.get()
               << ") path does not exist";
    return Status::INVALID_VALUE;
  }

  if (context_->backup_xml_path.get().empty()) {
    LOG(ERROR) << "Backup xml path was not set but is required";
    return Status::INVALID_VALUE;
  }

  // TODO(p.sikorski) check context_->uid.get()

  return Step::Status::OK;
}

void StepUnregisterApplication::BackupCertInfo() {
  std::string base64 = QueryCertificateAuthorCertificate(context_->pkgid.get(),
                                                         context_->uid.get());
  if (!base64.empty()) {
    ValidationCore::Base64Decoder decoder;
    decoder.append(base64);
    decoder.finalize();
    CertificateInfo certificate_info;
    certificate_info.author_certificate.set(ValidationCore::CertificatePtr(
        new ValidationCore::Certificate(decoder.get())));
    context_->certificate_info.set(certificate_info);
  }
}

Step::Status StepUnregisterApplication::process() {
  // Prepare certificate info for rollback operations
  BackupCertInfo();

  if (!UnregisterAppInPkgmgr(context_->xml_path.get(), context_->pkgid.get(),
                             context_->uid.get())) {
    LOG(ERROR) << "Failed to unregister package into database";
    return Status::ERROR;
  }

  LOG(DEBUG) << "Successfully unregister the application";

  return Status::OK;
}

Step::Status StepUnregisterApplication::undo() {
  if (!RegisterAppInPkgmgr(context_->backup_xml_path.get(),
                           context_->pkgid.get().c_str(),
                           context_->certificate_info.get(),
                           context_->uid.get())) {
    LOG(ERROR) << "Failed to restore the app registration in pkgmgr";
    // Continue to revert...
    return Step::Status::OK;
  }

  LOG(INFO) << "Successfully restored the app registration in pkgmgr";
  return Status::OK;
}

}  // namespace pkgmgr
}  // namespace common_installer
