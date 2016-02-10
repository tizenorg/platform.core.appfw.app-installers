// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_remove_manifest.h"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/system/error_code.hpp>

#include <pkgmgr-info.h>
#include <pkgmgr_installer.h>

#include <algorithm>
#include <string>

#include "common/utils/file_util.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace common_installer {
namespace pkgmgr {


common_installer::Step::Status StepRemoveManifest::process() {
  bs::error_code error;
  bf::remove(context_->xml_path.get(), error);

  if (error) {
    LOG(ERROR) << "Failed to remove xml manifest file";
    return Status::MANIFEST_ERROR;
  }
  LOG(DEBUG) << "Manifest file removed";
  return Status::OK;
}


}  // namespace pkgmgr
}  // namespace common_installer
