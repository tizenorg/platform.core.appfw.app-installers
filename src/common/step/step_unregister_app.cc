// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include <unistd.h>

#include <boost/filesystem.hpp>
#include <pkgmgr_installer.h>
#include <cassert>
#include <cstring>

#include "common/pkgmgr_registration.h"
#include "common/step/step_unregister_app.h"
#include "common/utils/file_util.h"

namespace common_installer {
namespace unregister_app {

namespace fs = boost::filesystem;

Step::Status StepUnregisterApplication::precheck() {
  if (context_->pkgid.get().empty()) {
    LOG(ERROR) << "pkgid attribute is empty";
    return Step::Status::INVALID_VALUE;
  }
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

Step::Status StepUnregisterApplication::process() {
  if (!UnregisterAppInPkgmgr(context_->xml_path.get(), context_->pkgid.get(),
                             context_->uid.get())) {
    LOG(ERROR) << "Failed to unregister package into database";
    return Status::ERROR;
  }

  LOG(DEBUG) << "Successfully unregister the application";

  return Status::OK;
}

}  // namespace unregister_app
}  // namespace common_installer
