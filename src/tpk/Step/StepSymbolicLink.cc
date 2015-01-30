#include <iostream>
#include <exception>
#include <boost/filesystem.hpp>

#include "StepSymbolicLink.h"
#include "common/step/step.h"
#include "common/app_installer.h"
#include "common/context_installer.h"
#include "common/utils.h"

#define DBG(msg) std::cout << "[Symoblic Link] " << msg << std::endl;
#define ERR(msg) std::cerr << "[ERROR: Symoblic Link] " << msg << std::endl;

/* Error codes to be returned */
static const common_installer::Step::Status
  &status_ok = common_installer::Step::Status::OK;
static const common_installer::Step::Status
  &status_error = common_installer::Step::Status::ERROR;


namespace tpk {
namespace Step {

namespace fs = boost::filesystem;


template <typename T>
int
StepSymbolicLink::createSymLink(T _app) {
  T app = _app;
  boost::system::error_code error;

  for(; app != nullptr; app=app->next) {
    fs::path bindir = fs::path(context_->pkg_path()) / fs::path(app->appid) / fs::path("bin");
    DBG("Creating dir: " << bindir);
    if (utils::CreateDir(bindir) == false) {
      ERR("Directory creation failure");
      return -1;
    }

    // Make a symlink with the name of appid, pointing exec file
    fs::path symlink_path = bindir / fs::path(app->appid);
    DBG("Creating symlink pointing " << symlink_path  << " to " <<  app->exec);
    fs::create_symlink(fs::path(app->exec), symlink_path, error);
    if(error) {
      ERR("Symlink creation failure");
      return -1;
    }

    // Give a execution permission to the original executable
    fs::path exec_path = bindir / fs::path(app->exec);
    DBG("Giving exe permission to " << exec_path);
    fs::permissions(exec_path, fs::owner_all |
        fs::group_read | fs::group_exe |
        fs::others_read | fs::others_exe, error);
    if(error) {
      ERR("Permission change failure");
      return -1;
    }
}
  return 0;
}

template <typename T>
int
StepSymbolicLink::removeSymLink(T _app) {
  /* NOTE: Unlike WRT app, tpk apps have bin/ directory by default. So we don't remove
   *       the bin/ directory itself.
   */
  T app = _app;

  for(; app != nullptr; app=app->next) {
    fs::path exec_path = fs::path(context_->pkg_path()) / fs::path(app->appid) / fs::path("bin");
    fs::remove_all(exec_path / fs::path(app->appid));
  }
}


common_installer::Step::Status
StepSymbolicLink::process() {

  // Get manifest_x
  manifest_x *m = context_->manifest_data();
  if(!m) {
    ERR("manifest_x is null");
    return status_error;
  }

  // get ui-app and service-app
  uiapplication_x *uiapp = m->uiapplication;
  serviceapplication_x *svcapp = m->serviceapplication;
  if(!(uiapp || svcapp)) {
    ERR("Neither ui-application nor service-application exists");
    return status_error;
  }
  if(createSymLink(uiapp)) return status_error;
  if(createSymLink(svcapp)) return status_error;

  return status_ok;
}


common_installer::Step::Status
StepSymbolicLink::clean() {
  return status_ok;
}


common_installer::Step::Status
StepSymbolicLink::undo() {
  manifest_x *m = context_->manifest_data();
  uiapplication_x *uiapp = m->uiapplication;
  serviceapplication_x *svcapp = m->serviceapplication;
  if(removeSymLink(uiapp)) return status_error;
  if(removeSymLink(svcapp)) return status_error;

  return status_ok;
}

} // namespace Step
} // namespace tpk

