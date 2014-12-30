/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "include/step/step_copy.h"

#include <cassert>
#include <cstring>
#include <iostream>

#include "include/utils.h"

#define DBG(msg) std::cout << "[Copy] " << msg << std::endl;
#define ERR(msg) std::cout << "[ERROR: Copy] " << msg << std::endl;

namespace fs = boost::filesystem;

int StepCopy::process(ContextInstaller* data) {
  assert(!data->pkgid().empty());

  data->set_pkg_path(
      data->GetApplicationPath() + std::string("/") + data->pkgid());

  // If there is only one app in the package,
  // all app files are stored in <pkg_path>/<mainapp_id>
  fs::path app_path_ = data->mfx_data()->uiapplication->next == NULL ?
      fs::path(data->pkg_path()) /= fs::path(data->mfx_data()->mainapp_id) :
      fs::path(data->pkg_path());

  if (!utils::CopyDir(fs::path(data->unpack_directory()), app_path_)) {
      ERR("Fail to copy tmp dir: " << data->unpack_directory()
          << " to dst dir: " << app_path_);
       return APPINST_R_ERROR;
  }

  DBG("Successfully copy: " << data->unpack_directory()
      << " to: " << data->pkg_path() << " directory");
  return APPINST_R_OK;
}

int StepCopy::clean(ContextInstaller* data) {
  DBG("Remove tmp dir: " << data->unpack_directory());
  fs::remove_all(data->unpack_directory());
  return APPINST_R_OK;
}

int StepCopy::undo(ContextInstaller* data) {
  if (fs::exists(data->pkg_path()))
    fs::remove_all(data->pkg_path());
  return APPINST_R_OK;
}
