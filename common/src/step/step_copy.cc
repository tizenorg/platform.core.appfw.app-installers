/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "include/step/step_copy.h"

#include <boost/filesystem.hpp>

#include <string>
#include <iostream>

#include "include/utils.h"

#define DBG(msg) std::cout << "[Copy] " << msg << std::endl;
#define ERR(msg) std::cout << "[ERROR: Copy] " << msg << std::endl;

namespace {

boost::filesystem::path pkgid_path;

}  // namespace

int StepCopy::process(ContextInstaller* data) {
  // FIXME: pkg_id will be known in step-parser, so use a test one now
  boost::filesystem::path pkgid("fake_pkgid");
  // boost::filesystem::path pkgid(data->pkgid());

  boost::filesystem::path tmp_path(data->GetApplicationPath());
  pkgid_path = tmp_path /= pkgid;

  boost::filesystem::path tmp_dir(data->unpack_directory());

  if (!utils::CopyDir(tmp_dir, pkgid_path)) {
    ERR("Fail to copy tmp dir: " << tmp_dir << " to dst dir: " << pkgid_path);
     return APPINST_R_ERROR;
  }

  DBG("Successfully copy: " << tmp_dir << " to: " << pkgid_path
      << " directory");
  return APPINST_R_OK;
}

int StepCopy::clean(ContextInstaller* data) {
  DBG("clean step");
  boost::filesystem::remove_all(data->unpack_directory());
  return APPINST_R_OK;
}

int StepCopy::undo(ContextInstaller* data) {
  if (boost::filesystem::exists(pkgid_path))
    boost::filesystem::remove_all(pkgid_path);
  return APPINST_R_OK;
}
