/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "include/step_copy.h"

#include <boost/filesystem.hpp>

#include <string>
#include <iostream>

#define DBG(msg) std::cout << "[Copy] " << msg << std::endl;
#define ERR(msg) std::cout << "[ERROR: Copy] " << msg << std::endl;

int StepCopy::process(ContextInstaller* data) {
  // boost::filesystem::path pkgid(data->pkgid());
  boost::filesystem::path pkgid("123456789");  // for testing...
  boost::filesystem::path tmp_path(data->GetApplicationPath());
  boost::filesystem::path pkgid_path = tmp_path /= pkgid;

  boost::filesystem::path tmp_dir(data->unpack_directory());

  if (!utils::CopyDir(tmp_dir, pkgid_path)) {
    ERR("Fail to copy tmp dir: " << tmp_dir << " to dst dir: " << pkgid_path);
    boost::filesystem::remove_all(pkgid_path);
     return APPINST_R_ERROR;
  }

  boost::filesystem::remove_all(data->unpack_directory());
  DBG("Successfully copy: " << tmp_dir << " to: " << pkgid_path
      << " directory");
  return APPINST_R_OK;
}

int StepCopy::clean(ContextInstaller* data) {
  DBG("clean step");
  return APPINST_R_OK;
}

int StepCopy::undo(ContextInstaller* data) {
  return APPINST_R_OK;
}
