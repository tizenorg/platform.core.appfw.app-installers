/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "common/step/step_copy.h"

#include <cassert>
#include <cstring>
#include <iostream>

#include "common/utils.h"

#define DBG(msg) std::cout << "[Copy] " << msg << std::endl;
#define ERR(msg) std::cout << "[ERROR: Copy] " << msg << std::endl;

namespace common_installer {
namespace copy {

namespace fs = boost::filesystem;

int StepCopy::process(ContextInstaller* data) {
  assert(!data->pkgid().empty());

  fs::path install_path = fs::path(data->GetApplicationPath()) /
      fs::path(data->pkgid());

  data->set_pkg_path(install_path.string());

  // FIXME: correctly order app's data.
  // If there is 1 app in package, app's data are stored in <pkg_path>/<app_id>
  // If there are >1 apps in package, app's data are stored in <pkg_path>
  // considering that multiple apps data are already separated in folders.
  if (!data->manifest_data()->uiapplication->next)
    install_path /= fs::path(data->manifest_data()->mainapp_id);

  if (!utils::CopyDir(fs::path(data->unpack_directory()), install_path)) {
    ERR("Fail to copy tmp dir: " << data->unpack_directory()
        << " to dst dir: " << install_path.string());
    return APPINST_R_ERROR;
  }

  DBG("Successfully copy: " << data->unpack_directory()
      << " to: " << install_path.string() << " directory");
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

}  // namespace copy
}  // namespace common_installer
