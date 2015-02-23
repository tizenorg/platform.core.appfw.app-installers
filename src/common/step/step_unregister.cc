// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include <unistd.h>

#include <boost/filesystem.hpp>
#include <cassert>
#include <cstring>

#include "common/step/step_unregister.h"
#include "utils/file_util.h"

namespace common_installer {
namespace unregister {

namespace fs = boost::filesystem;

Step::Status StepUnregister::process() {
  assert(!context_->pkgid().empty());

  const char* const appinst_tags[] = {"removable=true", nullptr, };

  int ret = context_->uid() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER) ?
      pkgmgr_parser_parse_usr_manifest_for_uninstallation(
          context_->xml_path().c_str(), context_->uid(),
          const_cast<char* const*>(appinst_tags)) :
      pkgmgr_parser_parse_manifest_for_uninstallation(
          context_->xml_path().c_str(), const_cast<char* const*>(appinst_tags));

  if (ret != 0) {
    LOG(ERROR) << "Failed to unregister package into database";
    return Status::ERROR;
  }
  LOG(DEBUG) << "Successfully unregister the application";

  return Status::OK;
}

Step::Status StepUnregister::clean() {
  LOG(DEBUG) << "Empty 'clean' method";
  return Status::OK;
}

Step::Status StepUnregister::undo() {
  LOG(DEBUG) << "Empty 'undo' method";
  return Status::OK;
}

}  // namespace unregister
}  // namespace common_installer
