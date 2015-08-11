/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#include "tpk/step/step_create_symbolic_link.h"
#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>
#include <pkgmgr-info.h>

#include <iostream>

#include "common/step/step.h"
#include "common/app_installer.h"
#include "common/context_installer.h"
#include "common/utils/file_util.h"
#include "common/utils/logging.h"


namespace tpk {
namespace filesystem {

namespace fs = boost::filesystem;
using common_installer::ContextInstaller;
typedef common_installer::Step::Status Status;

SCOPE_LOG_TAG(SymbolicLink)

namespace {

template <typename T>
bool CreateSymLink(T *app, ContextInstaller* context) {
  boost::system::error_code boost_error;

  for (; app != nullptr; app=app->next) {
    fs::path bindir = context->pkg_path.get() /
        fs::path("bin");
    LOG(INFO) << "Creating dir: " << bindir;
    if (!common_installer::CreateDir(bindir)) {
      LOG(ERROR) << "Directory creation failure: " << bindir;
      return false;
    }

    // Exec path
    // Make a symlink with the name of appid, pointing exec file
    fs::path symlink_path = bindir / fs::path(app->appid);
    LOG(INFO) << "Creating symlink " << symlink_path << " pointing " <<
        app->exec;
    fs::create_symlink(fs::path(app->exec), symlink_path, boost_error);
    if (boost_error) {
      LOG(ERROR) << "Symlink creation failure: " << symlink_path;
      return false;
    }

    // Give an execution permission to the original executable
    fs::path exec_path = bindir / fs::path(app->exec);
    LOG(INFO) << "Giving exec permission to " << exec_path;
    fs::permissions(exec_path, fs::owner_all |
        fs::group_read | fs::group_exe |
        fs::others_read | fs::others_exe, boost_error);
    if (boost_error) {
      LOG(ERROR) << "Permission change failure";
      return false;
    }
  }
  return true;
}


template <typename T>
bool RemoveSymLink(T *app, ContextInstaller* context) {
  /* NOTE: Unlike WRT app, tpk apps have bin/ directory by default.
   * So we don't remove the bin/ directory.
   */
  for (; app != nullptr; app=app->next) {
    fs::path exec_path = fs::path(context->pkg_path.get()) / fs::path("bin");
    fs::remove_all(exec_path / fs::path(app->appid));
  }
  return true;
}

}  // namespace

Status StepCreateSymbolicLink::precheck() {
  manifest_x *m = context_->manifest_data.get();
  if (!m) {
    LOG(ERROR) << "manifest_data attribute is empty";
    return Step::Status::INVALID_VALUE;
  }
  if (!(m->uiapplication || m->serviceapplication)) {
    LOG(ERROR) << "Neither ui-application nor service-application exists";
    return Step::Status::ERROR;
  }

  return Step::Status::OK;
}

Status StepCreateSymbolicLink::process() {
  // Get manifest_x
  manifest_x *m = context_->manifest_data.get();

  // get ui-app and service-app
  uiapplication_x *uiapp = m->uiapplication;
  serviceapplication_x *svcapp = m->serviceapplication;

  if (!CreateSymLink(uiapp, context_)) return Status::ERROR;
  if (!CreateSymLink(svcapp, context_)) return Status::ERROR;
  return Status::OK;
}


Status StepCreateSymbolicLink::undo() {
  manifest_x* m = context_->manifest_data.get();
  uiapplication_x *uiapp = m->uiapplication;
  serviceapplication_x *svcapp = m->serviceapplication;

  Step::Status ret = Status::OK;
  if (!RemoveSymLink(uiapp, context_)) {
    LOG(ERROR) << "Cannot remove Symboliclink for uiapp";
    ret = Status::ERROR;
  }
  if (!RemoveSymLink(svcapp, context_)) {
    LOG(ERROR) << "Cannot remove Symboliclink for svcapp";
    ret = Status::ERROR;
  }

  return ret;
}

}  // namespace filesystem
}  // namespace tpk
