// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_remove_zip_image.h"

#include <boost/filesystem/operations.hpp>
#include <boost/system/error_code.hpp>

#include <algorithm>
#include <string>
#include <vector>

#include "common/backup_paths.h"
#include "common/pkgmgr_registration.h"

namespace bs = boost::system;
namespace bf = boost::filesystem;

namespace common_installer {
namespace filesystem {

Step::Status StepRemoveZipImage::precheck() {
  if (context_->pkg_path.get().empty())
    LOG(ERROR) << "pkg_path attribute is empty";
  return Step::Status::OK;
}

Step::Status StepRemoveZipImage::process() {
  bool is_preloaded =
      strcmp(context_->manifest_data.get()->preload, "true") == 0;
  bf::path zip_image_path =
      GetZipPackageLocation(context_->pkgid.get(), is_preloaded);
  bs::error_code error;
  bf::remove(zip_image_path, error);
  return Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer
