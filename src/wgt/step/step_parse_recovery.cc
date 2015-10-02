// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_parse_recovery.h"

#include "common/backup_paths.h"
#include "common/installer_context.h"
#include "common/package_storage.h"
#include "common/request.h"

namespace {

const char kResWgtPath[] = "res/wgt";

}

namespace wgt {
namespace parse {

StepParseRecovery::StepParseRecovery(
    common_installer::InstallerContext* context)
    : StepParse(context, false) {
}

common_installer::Step::Status StepParseRecovery::process() {
  (void) StepParse::process();
  return Status::OK;
}

common_installer::Step::Status StepParseRecovery::precheck() {
  if (context_->root_application_path.get().empty()) {
    LOG(ERROR) << "Root path of packages in not set";
    return Status::ERROR;
  }
  if (context_->pkgid.get().empty()) {
    LOG(WARNING) << "Pkgid is not set. Parsing skipped";
    return Status::OK;
  }
  return Status::OK;
}

bool StepParseRecovery::LocateConfigFile() {
  // set package storage
  context_->package_storage =
      CreatePackageStorage(common_installer::RequestType::Recovery,
                           context_->root_application_path.get(),
                           context_->pkgid.get(),
                           "", nullptr);
  if (!context_->package_storage) {
    LOG(ERROR) << "Failed to create storage";
    return false;
  }

  if (Check(common_installer::GetBackupPathForPackagePath(
      context_->package_storage->path()) / kResWgtPath))
    return true;

  if (Check(context_->package_storage->path() / kResWgtPath))
    return true;

  return false;
}

}  // namespace parse
}  // namespace wgt
