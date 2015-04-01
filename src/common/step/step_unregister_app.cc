// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include <unistd.h>

#include <boost/filesystem.hpp>
#include <cassert>
#include <cstring>

#include "common/step/step_unregister_app.h"
#include "utils/file_util.h"

namespace common_installer {
namespace unregister_app {

namespace fs = boost::filesystem;

Step::Status StepUnregisterApplication::process() {
  assert(!context_->pkgid.get().empty());

  const char* const appinst_tags[] = {"removable=true", nullptr, };

  int ret = context_->uid.get() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER) ?
      pkgmgr_parser_parse_usr_manifest_for_uninstallation(
          context_->xml_path.get().c_str(), context_->uid.get(),
          const_cast<char* const*>(appinst_tags)) :
      pkgmgr_parser_parse_manifest_for_uninstallation(
          context_->xml_path.get().c_str(),
          const_cast<char* const*>(appinst_tags));

  if (ret != 0) {
    LOG(ERROR) << "Failed to unregister package into database";
    return Status::ERROR;
  }
  LOG(DEBUG) << "Successfully unregister the application";

  return Status::OK;
}

Step::Status StepUnregisterApplication::clean() {
  LOG(DEBUG) << "Empty 'clean' method";
  return Status::OK;
}

Step::Status StepUnregisterApplication::undo() {
  LOG(DEBUG) << "Empty 'undo' method";
  return Status::OK;
}

}  // namespace unregister_app
}  // namespace common_installer
