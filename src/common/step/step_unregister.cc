/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "common/step/step_unregister.h"

//TODO some of includes might not be needed
#include <unistd.h>

#include <boost/filesystem.hpp>
#include <cassert>
#include <cstring>
#include <iostream>
#include <cstdio>
#include <string>
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
