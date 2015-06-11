// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_update_app.h"

#include <pkgmgr-info.h>
#include <sys/types.h>
#include <unistd.h>

#include <boost/filesystem.hpp>
#include <vcore/Base64.h>
#include <vcore/Certificate.h>

#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>

#include "common/pkgmgr_registration.h"
#include "common/utils/file_util.h"

namespace bf = boost::filesystem;

namespace common_installer {
namespace update_app {

Step::Status StepUpdateApplication::precheck() {
  if (context_->xml_path.get().empty()) {
    LOG(ERROR) << "Xml path is empty";
    return Status::ERROR;
  }
  return Status::OK;
}

Step::Status StepUpdateApplication::process() {
  if (!UpgradeAppInPkgmgr(context_->xml_path.get(),
      context_->pkgid.get(), context_->certificate_info.get(),
      context_->uid.get())) {
    LOG(ERROR) << "Cannot upgrade manifest for application";
    return Status::ERROR;
  }

  LOG(INFO) << "Successfully install the application";
  return Status::OK;
}

Step::Status StepUpdateApplication::clean() {
  return Status::OK;
}

Step::Status StepUpdateApplication::undo() {
  // Prepare certification info for revert
  ValidationCore::Base64Decoder decoder;
  decoder.append(QueryCertificateAuthorCertificate(context_->pkgid.get(),
                                                   context_->uid.get()));
  decoder.finalize();
  CertificateInfo certificate_info;
  certificate_info.author_certificate.set(ValidationCore::CertificatePtr(
      new ValidationCore::Certificate(decoder.get())));

  if (!UpgradeAppInPkgmgr(context_->backup_xml_path.get(),
      context_->pkgid.get(), certificate_info,
      context_->uid.get())) {
    LOG(ERROR) << "Cannot revert manifest for application";
    return Status::ERROR;
  }
  LOG(INFO) << "Database reverted successfully";
  return Status::OK;
}

}  // namespace update_app
}  // namespace common_installer
