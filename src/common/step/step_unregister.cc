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

int StepUnregister::process(ContextInstaller* data) {
  assert(!data->pkgid().empty());

  char* const appinst_tags[] = {"removable=true", nullptr, };

  int ret = data->uid() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER) ?
      pkgmgr_parser_parse_usr_manifest_for_uninstallation(
          data->xml_path().c_str(), data->uid(), appinst_tags) :
      pkgmgr_parser_parse_manifest_for_uninstallation(
          data->xml_path().c_str(), appinst_tags);

  if (ret != 0) {
    DBG("Failed to unregister package into database");
    return APPINST_R_ERROR;
  }
  DBG("Successfully unregister the application");

  return APPINST_R_OK;
}

int StepUnregister::clean(ContextInstaller* data) {
  DBG("Empty 'clean' method");
  return APPINST_R_OK;
}

int StepUnregister::undo(ContextInstaller* data) {
  DBG("Empty 'undo' method");
  return APPINST_R_OK;
}

}  // namespace unregister
}  // namespace common_installer
