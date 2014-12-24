/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "include/step_copy.h"

#include <boost/filesystem.hpp>

#include <string>
#include <iostream>

#define DBG(msg) std::cout << "[Copy] " << msg << std::endl;
#define ERR(msg) std::cout << "[ERROR: Copy] " << msg << std::endl;

int StepCopy::process(ContextInstaller* data) {
  DBG("process step: pkgid: " << data->pkgid());
  //boost::filesystem::path pkgid(data->pkgid());
  boost::filesystem::path pkgid("123456789"); // for testing...
  boost::filesystem::path tmp_path(data->GetApplicationPath());
  boost::filesystem::path pkgid_path = tmp_path /= pkgid;

  DBG("pkgid_path: " << pkgid_path.string());
//  utils::CreateDir(pkgid_path);

  boost::system::error_code error;
  boost::filesystem::path tmp_dir(data->unpack_directory());
  boost::filesystem::copy_directory(tmp_dir, pkgid_path, error);
  if (error) {
    ERR("Fail to copy: " << tmp_dir.string() << " to: " << pkgid_path.string()
        << " (error: " << boost::system::system_error(error).what() << ")");
    return APPINST_R_ERROR;
  }

  DBG("Successfully moves: " << tmp_dir << " to: " << pkgid_path
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
