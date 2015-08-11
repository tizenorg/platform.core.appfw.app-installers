/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "common/step/step_register_app.h"

#include <unistd.h>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <string>

#include "common/pkgmgr_registration.h"
#include "common/utils/file_util.h"

namespace common_installer {
namespace pkgmgr {

namespace fs = boost::filesystem;

Step::Status StepRegisterApplication::precheck() {
  if (context_->xml_path.get().empty()) {
    LOG(ERROR) << "xml_path attribute is empty";
    return Step::Status::INVALID_VALUE;
  }
  if (!boost::filesystem::exists(context_->xml_path.get())) {
    LOG(ERROR) << "xml_path ("
               << context_->xml_path.get()
               << ") path does not exist";
    return Step::Status::INVALID_VALUE;
  }
  // TODO(p.sikorski) check context_->uid.get()

  return Step::Status::OK;
}

Step::Status StepRegisterApplication::process() {
  if (!RegisterAppInPkgmgr(context_->xml_path.get(),
                           context_->pkgid.get().c_str(),
                           context_->certificate_info.get(),
                           context_->uid.get())) {
    LOG(ERROR) << "Failed to register the app";
    return Step::Status::ERROR;
  }

  LOG(INFO) << "Successfully registered the app";
  return Status::OK;
}

Step::Status StepRegisterApplication::undo() {
  if (!UnregisterAppInPkgmgr(context_->xml_path.get(), context_->pkgid.get(),
                           context_->uid.get())) {
    LOG(ERROR) << "Application couldn't be unregistered";
    return Status::ERROR;
  }

  LOG(INFO) << "Successfuly clean database";
  return Status::OK;
}

}  // namespace pkgmgr
}  // namespace common_installer
