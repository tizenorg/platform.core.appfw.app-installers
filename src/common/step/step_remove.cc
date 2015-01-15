/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "common/step/step_remove.h"

//TODO not sure if all includes are used
#include <cassert>
#include <cstring>
#include <iostream>
#include <string>

#include "common/utils.h"

#define DBG(msg) std::cout << "[Remove] " << msg << std::endl;
#define ERR(msg) std::cout << "[ERROR: Remove] " << msg << std::endl;

namespace common_installer {
namespace remove {

namespace fs = boost::filesystem;

int StepRemove::process(ContextInstaller* data) {
  assert(!data->pkgid().empty());

  //TODO this code is copy pasted. Maybe it could be added to set_pkg_path method?
  fs::path install_path = fs::path(data->GetApplicationPath()) /
      fs::path(data->pkgid());

  data->set_pkg_path(install_path.string());
  
  DBG("Removing dir: " << data->pkg_path());
  
  //TODO if? or assert?
  if (fs::exists(data->pkg_path()))
    fs::remove_all(data->pkg_path());
  return APPINST_R_OK;
}

int StepCopy::clean(ContextInstaller* data) {
  DBG("Empty 'clean' method");
  return APPINST_R_OK;
}

int StepCopy::undo(ContextInstaller* data) {
  DBG("Empty 'undo' method");
  return APPINST_R_OK;
}

}  // namespace remove
}  // namespace common_installer
