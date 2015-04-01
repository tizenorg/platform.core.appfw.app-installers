/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#include "tpk/step/step_create_symbolic_link.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include "common/step/step.h"
#include "common/app_installer.h"
#include "common/context_installer.h"
#include "utils/file_util.h"
#include "utils/logging.h"


namespace tpk {
namespace step {

namespace fs = boost::filesystem;
using common_installer::ContextInstaller;
typedef common_installer::Step::Status Status;

SCOPE_LOG_TAG(SymbolicLink)

namespace {

template <typename T>
bool CreateSymLink(T *app, ContextInstaller* context) {
  boost::system::error_code error;

  for (; app != nullptr; app=app->next) {
    fs::path bindir = fs::path(context->pkg_path.get()) / fs::path(app->appid) /
        fs::path("bin");
    LOG(INFO) << "Creating dir: " << bindir;
    if (!common_installer::utils::CreateDir(bindir)) {
      LOG(ERROR) << "Directory creation failure";
      return false;
    }

    // Make a symlink with the name of appid, pointing exec file
    fs::path symlink_path = bindir / fs::path(app->appid);
    LOG(INFO) << "Creating symlink " << symlink_path << " pointing " <<
        app->exec;
    fs::create_symlink(fs::path(app->exec), symlink_path, error);
    if (error) {
      LOG(ERROR) << "Symlink creation failure";
      return false;
    }

    // Give a execution permission to the original executable
    fs::path exec_path = bindir / fs::path(app->exec);
    LOG(INFO) << "Giving exec permission to " << exec_path;
    fs::permissions(exec_path, fs::owner_all |
        fs::group_read | fs::group_exe |
        fs::others_read | fs::others_exe, error);
    if (error) {
      LOG(ERROR) << "Permission change failure";
      return false;
    }
  }
  return true;
}


template <typename T>
bool RemoveSymLink(T *app, ContextInstaller* context) {
  /* NOTE: Unlike WRT app, tpk apps have bin/ directory by default.
   * So we don't remove the bin/ directory itself.
   */
  for (; app != nullptr; app=app->next) {
    fs::path exec_path = fs::path(context->pkg_path.get()) /
        fs::path(app->appid) / fs::path("bin");
    fs::remove_all(exec_path / fs::path(app->appid));
  }
  return true;
}

}  // namespace



Status StepCreateSymbolicLink::process() {
  // Get manifest_x
  manifest_x *m = context_->manifest_data.get();
  if (!m) {
    LOG(ERROR) << "manifest_x is null";
    return Status::ERROR;
  }

  // get ui-app and service-app
  uiapplication_x *uiapp = m->uiapplication;
  serviceapplication_x *svcapp = m->serviceapplication;
  if (!(uiapp || svcapp)) {
    LOG(ERROR) << "Neither ui-application nor service-application exists";
    return Status::ERROR;
  }

  if (!CreateSymLink(uiapp, context_)) return Status::ERROR;
  if (!CreateSymLink(svcapp, context_)) return Status::ERROR;
  return Status::OK;
}


Status StepCreateSymbolicLink::clean() {
  return Status::OK;
}


Status StepCreateSymbolicLink::undo() {
  manifest_x* m = context_->manifest_data.get();
  uiapplication_x *uiapp = m->uiapplication;
  serviceapplication_x *svcapp = m->serviceapplication;
  if (!RemoveSymLink(uiapp, context_)) return Status::ERROR;
  if (!RemoveSymLink(svcapp, context_)) return Status::ERROR;

  return Status::OK;
}

}  // namespace step
}  // namespace tpk
