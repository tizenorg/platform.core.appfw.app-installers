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

    // Icon path
    // Make an Icon symlink, if the icon exists
    if (app->icon && app->icon->name) {
      // Check if the icon file actually exists
      fs::path src_icon_path = fs::path(context->pkg_path.get())
          / fs::path(app->icon->name);
      if (fs::exists(src_icon_path)) {
        // copy icon to the destination dir
        fs::path dest_icon_dir = fs::path(getIconPath(context->uid.get()));
        fs::path dest_icon_filename = fs::path(app->appid) +=
            fs::path(app->icon->name).extension();
        if (!common_installer::CreateDir(dest_icon_dir)) {
          LOG(ERROR) << "Directory creation failure: " << dest_icon_dir;
          return false;
        }
        fs::path dest_icon_path = dest_icon_dir;
        dest_icon_path /= dest_icon_filename;
        LOG(INFO) << "Creating symlink " << dest_icon_path << " pointing " <<
            src_icon_path;
        fs::create_symlink(src_icon_path, dest_icon_path, boost_error);
        if (boost_error) {
          LOG(ERROR) << "Symlink creation failure: " << dest_icon_path;
          return false;
        }
      } else {
        LOG(WARNING) << "No icon file found in the package: " << src_icon_path;
      }
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


Status StepCreateSymbolicLink::undo() {
  manifest_x* m = context_->manifest_data.get();
  uiapplication_x *uiapp = m->uiapplication;
  serviceapplication_x *svcapp = m->serviceapplication;
  if (!RemoveSymLink(uiapp, context_)) return Status::ERROR;
  if (!RemoveSymLink(svcapp, context_)) return Status::ERROR;

  return Status::OK;
}

}  // namespace filesystem
}  // namespace tpk
