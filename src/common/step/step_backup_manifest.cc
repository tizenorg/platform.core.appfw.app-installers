// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_backup_manifest.h"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/system/error_code.hpp>

#include <pkgmgr-info.h>
#include <pkgmgr_installer.h>

#include <algorithm>
#include <string>

#include "common/utils/file_util.h"
#include "common/utils/logging.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace common_installer {
namespace backup {

Step::Status StepBackupManifest::precheck() {
  if (!bf::exists(context_->xml_path.get())) {
    LOG(ERROR) << "Xml manifest file does not exist";
    return Status::ERROR;
  }
  return Status::OK;
}

Step::Status StepBackupManifest::process() {
  // set backup file path
  bf::path backup_xml_path = context_->xml_path.get();
  backup_xml_path += ".bck";
  context_->backup_xml_path.set(backup_xml_path);
  bs::error_code error;
  bf::copy(context_->xml_path.get(), context_->backup_xml_path.get(), error);
  if (error) {
    LOG(ERROR) << "Failed to make a copy of xml manifest file";
    return Status::ERROR;
  }
  LOG(DEBUG) << "Manifest backup created";
  return Status::OK;
}

Step::Status StepBackupManifest::clean() {
  bs::error_code error;
  bf::remove(context_->backup_xml_path.get(), error);
  if (error) {
    LOG(WARNING) << "Cannot remove backup manifest file";
    return Status::ERROR;
  }
  LOG(DEBUG) << "Manifest backup removed";
  return Status::OK;
}

Step::Status StepBackupManifest::undo() {
  if (bf::exists(context_->backup_xml_path.get())) {
    bs::error_code error;
    bf::remove(context_->xml_path.get(), error);
    if (error) {
      LOG(ERROR) << "Failed to remove newly generated xml file in revert";
      return Status::ERROR;
    }
    if (!MoveFile(context_->backup_xml_path.get(),
        context_->xml_path.get())) {
      LOG(ERROR) << "Failed to revert a content of xml manifest file";
      return Status::ERROR;
    }
    LOG(DEBUG) << "Manifest reverted from backup";
  }
  return Status::OK;
}

}  // namespace backup
}  // namespace common_installer
