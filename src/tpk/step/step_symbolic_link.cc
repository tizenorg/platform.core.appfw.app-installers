/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#include "tpk/step/step_symbolic_link.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include "common/step/step.h"
#include "common/app_installer.h"
#include "common/context_installer.h"
#include "common/utils.h"
#include "utils/logging.h"

/* Error codes to be returned */
static const common_installer::Step::Status
  &status_ok = common_installer::Step::Status::OK;
static const common_installer::Step::Status
  &status_error = common_installer::Step::Status::ERROR;

namespace tpk {
namespace step {

namespace fs = boost::filesystem;
using common_installer::ContextInstaller;

SCOPE_LOG_TAG(SymbolicLink)

template <typename T>
static int createSymLink(T _app, ContextInstaller* context_) {
  T app = _app;
  boost::system::error_code error;

  for (; app != nullptr; app=app->next) {
    fs::path bindir = fs::path(context_->pkg_path()) / fs::path(app->appid) /
        fs::path("bin");
    LOG(INFO) << "Creating dir: " << bindir;
    if (utils::CreateDir(bindir) == false) {
      LOG(ERROR) << "Directory creation failure";
      return -1;
    }

    // Make a symlink with the name of appid, pointing exec file
    fs::path symlink_path = bindir / fs::path(app->appid);
    LOG(INFO) << "Creating symlink pointing " << symlink_path  << " to " <<
        app->exec;
    fs::create_symlink(fs::path(app->exec), symlink_path, error);
    if (error) {
      LOG(ERROR) << "Symlink creation failure";
      return -1;
    }

    // Give a execution permission to the original executable
    fs::path exec_path = bindir / fs::path(app->exec);
    LOG(INFO) << "Giving exec permission to " << exec_path;
    fs::permissions(exec_path, fs::owner_all |
        fs::group_read | fs::group_exe |
        fs::others_read | fs::others_exe, error);
    if (error) {
      LOG(ERROR) << "Permission change failure";
      return -1;
    }
}
  return 0;
}


template <typename T>
static int removeSymLink(T _app, ContextInstaller* context_) {
  /* NOTE: Unlike WRT app, tpk apps have bin/ directory by default.
   * So we don't remove the bin/ directory itself.
   */
  T app = _app;

  for (; app != nullptr; app=app->next) {
    fs::path exec_path = fs::path(context_->pkg_path()) /
        fs::path(app->appid) / fs::path("bin");
    fs::remove_all(exec_path / fs::path(app->appid));
  }
}


common_installer::Step::Status StepSymbolicLink::process() {
  // Get manifest_x
  manifest_x *m = context_->manifest_data();
  if (!m) {
    LOG(ERROR) << "manifest_x is null";
    return status_error;
  }

  // get ui-app and service-app
  uiapplication_x *uiapp = m->uiapplication;
  serviceapplication_x *svcapp = m->serviceapplication;
  if (!(uiapp || svcapp)) {
    LOG(ERROR) << "Neither ui-application nor service-application exists";
    return status_error;
  }
  if (createSymLink(uiapp, context_)) return status_error;
  if (createSymLink(svcapp, context_)) return status_error;

  return status_ok;
}


common_installer::Step::Status StepSymbolicLink::clean() {
  return status_ok;
}


common_installer::Step::Status StepSymbolicLink::undo() {
  manifest_x *m = context_->manifest_data();
  uiapplication_x *uiapp = m->uiapplication;
  serviceapplication_x *svcapp = m->serviceapplication;
  if (removeSymLink(uiapp, context_)) return status_error;
  if (removeSymLink(svcapp, context_)) return status_error;

  return status_ok;
}

}  // namespace step
}  // namespace tpk
