// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "common/step/step_update_app.h"

#include <pkgmgr-info.h>
#include <sys/types.h>
#include <unistd.h>

#include <boost/filesystem.hpp>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>

#include "utils/file_util.h"

namespace bf = boost::filesystem;

namespace {

const char* const kAppinstTags[] = {"removable=true", nullptr, };

bool UpgradeManifestInformation(uid_t uid, const bf::path& xml_path) {
  int ret = uid != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER) ?
       pkgmgr_parser_parse_usr_manifest_for_upgrade(
           xml_path.string().c_str(), uid,
           const_cast<char* const*>(kAppinstTags)) :
       pkgmgr_parser_parse_usr_manifest_for_upgrade(
           xml_path.string().c_str(), uid,
           const_cast<char* const*>(kAppinstTags));
  return !ret;
}

}  // anonymous namespace

namespace common_installer {
namespace update_app {

Step::Status StepUpdateApplication::precheck() {
  if (context_->xml_path.get().empty()) {
    LOG(ERROR) << "Xml path is empty";
    return Status::ERROR;
  }
  return Status::OK;
}

Step::Status StepUpdateApplication::process() {
  if (!UpgradeManifestInformation(context_->uid.get(),
      context_->xml_path.get())) {
    LOG(ERROR) << "Cannot upgrade manifest for application";
    return Status::ERROR;
  }

  LOG(INFO) << "Successfully install the application";
  return Status::OK;
}

Step::Status StepUpdateApplication::clean() {
  return Status::OK;
}

Step::Status StepUpdateApplication::undo() {
  if (!UpgradeManifestInformation(context_->uid.get(),
      context_->backup_xml_path.get())) {
    LOG(ERROR) << "Cannot revert manifest for application";
    return Status::ERROR;
  }
  LOG(INFO) << "Database reverted successfully";
  return Status::OK;
}

}  // namespace update_app
}  // namespace common_installer
