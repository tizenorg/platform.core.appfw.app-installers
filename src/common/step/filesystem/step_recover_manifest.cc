// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/filesystem/step_recover_manifest.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>
#include <pkgmgr-info.h>

#include "common/paths.h"
#include "common/utils/file_util.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace common_installer {
namespace filesystem {

Step::Status StepRecoverManifest::RecoveryNew() {
  if (!SetXmlPaths()) {
    LOG(DEBUG) << "Manifest recovery not needed";
    return Status::OK;
  }
  if (bf::exists(context_->xml_path.get())) {
    bs::error_code error;
    bf::remove(context_->xml_path.get(), error);
  }
  LOG(INFO) << "Manifest recovery done";
  return Status::OK;
}

Step::Status StepRecoverManifest::RecoveryUpdate() {
  if (!SetXmlPaths()) {
    LOG(DEBUG) << "Manifest recovery not needed";
    return Status::OK;
  }
  if (bf::exists(context_->backup_xml_path.get())) {
    if (bf::exists(context_->xml_path.get())) {
      bs::error_code error;
      bf::remove(context_->xml_path.get(), error);
      if (error) {
        LOG(ERROR) << "Cannot move manifest file to restore its location";
        return Status::RECOVERY_ERROR;
      }
    }
    (void) MoveFile(context_->backup_xml_path.get(), context_->xml_path.get());
  }
  LOG(INFO) << "Manifest recovery done";
  return Status::OK;
}

bool StepRecoverManifest::SetXmlPaths() {
  if (context_->pkgid.get().empty())
    return false;
  bf::path xml_path =
      bf::path(getUserManifestPath(context_->uid.get(),
          context_->is_preload_request.get()))
      / context_->pkgid.get();
  xml_path += ".xml";
  context_->xml_path.set(xml_path);
  context_->backup_xml_path.set(GetBackupPathForManifestFile(xml_path));
  return true;
}

}  // namespace filesystem
}  // namespace common_installer
