/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "common/step/pkgmgr/step_register_app.h"

#include <unistd.h>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <string>

#include "common/pkgmgr_registration.h"
#include "common/utils/file_util.h"

namespace common_installer {
namespace pkgmgr {

Step::Status StepRegisterApplication::precheck() {
  if (context_->xml_path.get().empty()) {
    LOG(ERROR) << "xml_path attribute is empty";
    return Step::Status::MANIFEST_NOT_FOUND;
  }
  if (!boost::filesystem::exists(context_->xml_path.get())) {
    LOG(ERROR) << "xml_path ("
               << context_->xml_path.get()
               << ") path does not exist";
    return Step::Status::MANIFEST_NOT_FOUND;
  }

  return Step::Status::OK;
}

Step::Status StepRegisterApplication::process() {
  if (!RegisterAppInPkgmgr(context_->manifest_data.get(),
                           context_->xml_path.get(),
                           context_->pkgid.get(),
                           context_->certificate_info.get(),
                           context_->uid.get(),
                           context_->request_mode.get(),
                           context_->tep_path.get())) {
    LOG(ERROR) << "Failed to register the app";
    return Step::Status::REGISTER_ERROR;
  }

  LOG(INFO) << "Successfully registered the app";
  return Status::OK;
}

Step::Status StepRegisterApplication::undo() {
  if (!UnregisterAppInPkgmgr(context_->manifest_data.get(),
                             context_->xml_path.get(),
                             context_->pkgid.get(),
                             context_->uid.get(),
                             context_->request_mode.get())) {
    LOG(ERROR) << "Application couldn't be unregistered";
    return Status::REGISTER_ERROR;
  }

  LOG(INFO) << "Successfuly clean database";
  return Status::OK;
}

}  // namespace pkgmgr
}  // namespace common_installer
