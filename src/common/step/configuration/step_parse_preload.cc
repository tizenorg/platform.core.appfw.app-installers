// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/configuration/step_parse_preload.h"

#include <pkgmgr-info.h>
#include <unistd.h>

#include <boost/filesystem.hpp>

#include <cstring>
#include <cstdio>
#include <string>

#include "common/pkgmgr_interface.h"
#include "common/utils/file_util.h"

namespace bf = boost::filesystem;
namespace ci = common_installer;

namespace common_installer {
namespace configuration {

ci::Step::Status StepParsePreload::process() {
  const char* preload_manifest_val = context_->manifest_data.get()->preload;

  if (strcmp(preload_manifest_val, "true") != 0) {
    bool is_preload = context_->is_preload_request.get();

    LOG(INFO) << "is_preload : (" << is_preload << ")";
    if (is_preload) {
      context_->manifest_data.get()->preload = strdup("true");

      if (getuid() != 0) {
        LOG(ERROR) << "You're not authorized to install preload app: "
            << context_->pkgid.get().c_str();
        return Status::OPERATION_NOT_ALLOWED;
      }
    } else {
      context_->manifest_data.get()->preload = strdup("false");
    }
  }

  return Status::OK;
}

}  // namespace configuration
}  // namespace common_installer
