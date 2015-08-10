// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_parse_recovery.h"

#include "common/backup_paths.h"
#include "common/context_installer.h"

namespace {

const char kResWgtPath[] = "res/wgt";

}

namespace wgt {
namespace parse {

StepParseRecovery::StepParseRecovery(
    common_installer::ContextInstaller* context)
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
  context_->pkg_path.set(
      context_->root_application_path.get() / context_->pkgid.get());

  if (Check(common_installer::GetBackupPathForPackagePath(
      context_->pkg_path.get()) / kResWgtPath))
    return true;

  if (Check(context_->pkg_path.get() / kResWgtPath))
    return true;

  return false;
}

}  // namespace parse
}  // namespace wgt
