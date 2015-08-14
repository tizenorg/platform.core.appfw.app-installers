// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "tpk/step/step_parse_recovery.h"

#include <boost/filesystem/operations.hpp>

#include "common/backup_paths.h"

namespace bf = boost::filesystem;

namespace {

const char kManifestFileName[] = "tizen-manifest.xml";

}  // namespace

namespace tpk {
namespace parse {

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

boost::filesystem::path StepParseRecovery::LocateConfigFile() const {
  context_->pkg_path.set(
      context_->root_application_path.get() / context_->pkgid.get());
  bf::path path1 = common_installer::GetBackupPathForPackagePath(
      context_->pkg_path.get()) / kManifestFileName;
  bf::path path2 = context_->pkg_path.get() / kManifestFileName;
  if (bf::exists(path1))
    return path1;
  if (bf::exists(path2))
    return path2;
  return {};
}

}  // namespace parse
}  // namespace tpk

