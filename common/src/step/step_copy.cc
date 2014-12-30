/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "include/step/step_copy.h"

#include <string>
#include <iostream>

#include "include/utils.h"

#define DBG(msg) std::cout << "[Copy] " << msg << std::endl;
#define ERR(msg) std::cout << "[ERROR: Copy] " << msg << std::endl;


int StepCopy::process(ContextInstaller* data) {
  // FIXME: pkg_id will be known in step-parser, so use a test one now
  //pkg_path_ = path(data->GetApplicationPath()) /= path(data->pkgid());
  pkg_path_ = path(data->GetApplicationPath()) /= path("fake_pkgid");

  if (!utils::CopyDir(path(data->unpack_directory()), pkg_path_)) {
    ERR("Fail to copy tmp dir: " << data->unpack_directory() << " to dst dir: " << pkg_path_);
     return APPINST_R_ERROR;
  }

  DBG("Successfully copy: " << data->unpack_directory() << " to: " << pkg_path_
      << " directory");
  return APPINST_R_OK;
}

int StepCopy::clean(ContextInstaller* data) {
  DBG("Remove tmp dir: " << data->unpack_directory());
  boost::filesystem::remove_all(data->unpack_directory());
  return APPINST_R_OK;
}

int StepCopy::undo(ContextInstaller* data) {
  if (boost::filesystem::exists(pkg_path_))
    boost::filesystem::remove_all(pkg_path_);
  return APPINST_R_OK;
}
