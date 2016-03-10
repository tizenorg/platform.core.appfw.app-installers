// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/pkgmgr/step_unregister_app.h"

#include <unistd.h>

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>
#include <pkgmgr_installer.h>
#include <vcore/Certificate.h>

#include <cassert>
#include <string>

#include "common/pkgmgr_registration.h"
#include "common/utils/file_util.h"

namespace common_installer {
namespace pkgmgr {

namespace bs = boost::system;
namespace bf = boost::filesystem;

Step::Status StepUnregisterApplication::precheck() {
  if (context_->pkgid.get().empty()) {
    LOG(ERROR) << "pkgid attribute is empty";
    return Status::PACKAGE_NOT_FOUND;
  }
  if (context_->xml_path.get().empty()) {
    LOG(ERROR) << "xml_path attribute is empty";
    return Status::MANIFEST_NOT_FOUND;
  }
  if (!boost::filesystem::exists(context_->xml_path.get())) {
    LOG(ERROR) << "xml_path ("
               << context_->xml_path.get()
               << ") path does not exist";
    return Status::MANIFEST_NOT_FOUND;
  }

  return Step::Status::OK;
}

bool StepUnregisterApplication::BackupCertInfo() {
  std::string base64 = QueryCertificateAuthorCertificate(context_->pkgid.get(),
                                                         context_->uid.get());
  if (!base64.empty()) {
    CertificateInfo certificate_info;
    try {
      certificate_info.author_certificate.set(ValidationCore::CertificatePtr(
          new ValidationCore::Certificate(
              base64,
              ValidationCore::Certificate::FormType::FORM_BASE64)));
    } catch (const ValidationCore::Certificate::Exception::Base &e) {
      LOG(ERROR) << "Exception in cert-svc-vcore Certificate "
                 << "Dump : " << e.DumpToString();
      return false;
    }
    context_->certificate_info.set(certificate_info);
  }

  return true;
}

Step::Status StepUnregisterApplication::process() {
  // Prepare certificate info for rollback operations
  if (!BackupCertInfo()) {
    LOG(ERROR) << "Failed to backup cert info";
  }

  if (!UnregisterAppInPkgmgr(context_->manifest_data.get(),
                             context_->xml_path.get(),
                             context_->pkgid.get(),
                             context_->uid.get(),
                             context_->request_mode.get())) {
    LOG(ERROR) << "Failed to unregister package into database";
  }

  // remove manifest file
  bs::error_code error;
  bf::remove(context_->xml_path.get(), error);

  LOG(DEBUG) << "Successfully unregister the application";

  return Status::OK;
}

Step::Status StepUnregisterApplication::undo() {
  if (!RegisterAppInPkgmgr(context_->manifest_data.get(),
                           context_->backup_xml_path.get(),
                           context_->pkgid.get(),
                           context_->certificate_info.get(),
                           context_->uid.get(),
                           context_->request_mode.get())) {
    LOG(ERROR) << "Failed to restore the app registration in pkgmgr";
    return Step::Status::REGISTER_ERROR;
  }

  LOG(INFO) << "Successfully restored the app registration in pkgmgr";
  return Status::OK;
}

}  // namespace pkgmgr
}  // namespace common_installer
