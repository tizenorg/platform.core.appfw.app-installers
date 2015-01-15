/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "common/step/step_unregister.h"

//TODO some of includes might not be needed
#include <pkgmgr-info.h>
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

  //TODO unregister api call
  
  
  DBG("TODO Call pkgmgr API to unregister app");
  
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
