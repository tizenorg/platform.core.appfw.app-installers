// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include <unistd.h>

#include <boost/filesystem.hpp>
#include <cassert>
#include <cstring>
#include <iostream>

#include "common/step/step_unregister.h"
#include "common/utils.h"
#define DBG(msg) std::cout << "[Unregister] " << msg << std::endl;
#define ERR(msg) std::cout << "[ERROR: Unregister] " << msg << std::endl;

namespace common_installer {
namespace unregister {

namespace fs = boost::filesystem;

Step::Status StepUnregister::process() {
  assert(!context_->pkgid().empty());

  char* const appinst_tags[] = {"removable=true", nullptr, };

  int ret = context_->uid() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER) ?
      pkgmgr_parser_parse_usr_manifest_for_uninstallation(
          context_->xml_path().c_str(), context_->uid(), appinst_tags) :
      pkgmgr_parser_parse_manifest_for_uninstallation(
          context_->xml_path().c_str(), appinst_tags);

  if (ret != 0) {
    ERR("Failed to unregister package into database");
    return Status::ERROR;
  }
  DBG("Successfully unregister the application");

  return Status::OK;
}

Step::Status StepUnregister::clean() {
  DBG("Empty 'clean' method");
  return Status::OK;
}

Step::Status StepUnregister::undo() {
  DBG("Empty 'undo' method");
  return Status::OK;
}

}  // namespace unregister
}  // namespace common_installer
