// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_update_tep.h"

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

Step::Status StepUpdateTep::process() {
  if (context_->tep_path.get().empty())
    return Status::OK;

  if (!UpdateTepInfoInPkgmgr(context_->tep_path.get(),
                          context_->pkgid.get(),
                          context_->uid.get(),
                          context_->request_mode.get())) {
    LOG(ERROR) << "Cannot update tep info for application";
    return Status::REGISTER_ERROR;
  }

  LOG(INFO) << "Successfully update the tep info for application";
  return Status::OK;
}

Step::Status StepUpdateTep::undo() {
  return Status::OK;
}

}  // namespace pkgmgr
}  // namespace common_installer
