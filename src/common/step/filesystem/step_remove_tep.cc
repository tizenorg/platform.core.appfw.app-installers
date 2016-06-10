// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_remove_tep.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace common_installer {
namespace filesystem {

Step::Status StepRemoveTep::process() {
  if (context_->manifest_data.get()->tep_name) {
    bf::path tep_path(context_->manifest_data.get()->tep_name);
    if (bf::exists(tep_path)) {
      bs::error_code error;
      bf::remove(tep_path, error);
      if (error) {
        // Deinstallation should not fail even if sd card is not inserted
        // In spite of this, we print only warning
        LOG(WARNING) << "Failed to remove tep file from: " << tep_path;
      }
    }
    LOG(DEBUG) << "Tep package file removed: " << tep_path;
  }
  return Status::OK;
}

}  // namespace filesystem
}  // namespace common_installer
