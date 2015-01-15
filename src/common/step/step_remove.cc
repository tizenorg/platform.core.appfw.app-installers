/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "common/step/step_remove.h"

#include <pkgmgr-info.h>

#include <cstring>
#include <iostream>

#include "common/utils.h"

#define DBG(msg) std::cout << "[Remove] " << msg << std::endl;
#define ERR(msg) std::cout << "[ERROR: Remove] " << msg << std::endl;

namespace common_installer {
namespace remove {

namespace fs = boost::filesystem;

int StepRemove::process(ContextInstaller* data) {
  uiapplication_x* ui = data->manifest_data()->uiapplication;

  if (!fs::exists(data->pkg_path()))
    DBG("dir: " << data->pkg_path() << "not exist");

  fs::remove_all(data->pkg_path());
  for (; ui != nullptr; ui = ui->next) {
    fs::path app_icon = fs::path(getIconPath(data->uid()))
      / fs::path(ui->appid);
    app_icon += fs::path(".png");
    if (fs::exists(app_icon))
      fs::remove_all(app_icon);
  }
  fs::remove_all(data->xml_path());

  DBG("Removing dir: " << data->pkg_path());


  return APPINST_R_OK;
}

int StepRemove::clean(ContextInstaller* data) {
  return APPINST_R_OK;
}

int StepRemove::undo(ContextInstaller* data) {
  return APPINST_R_OK;
}

}  // namespace remove
}  // namespace common_installer
