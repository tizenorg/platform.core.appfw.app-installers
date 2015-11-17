/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#include "tpk/step/step_create_symbolic_link.h"
#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>
#include <pkgmgr-info.h>

#include <iostream>

#include "common/step/step.h"
#include "common/app_installer.h"
#include "common/installer_context.h"
#include "common/utils/file_util.h"
#include "common/utils/glist_range.h"
#include "common/utils/logging.h"

namespace tpk {
namespace filesystem {

namespace bf = boost::filesystem;
using common_installer::InstallerContext;
typedef common_installer::Step::Status Status;

namespace {

bool CreateSymLink(application_x* app, InstallerContext* context) {
  boost::system::error_code boost_error;
  bf::path bindir = context->pkg_path.get() /
      bf::path("bin");
  LOG(DEBUG) << "Creating dir: " << bindir;
  if (!common_installer::CreateDir(bindir)) {
    LOG(ERROR) << "Directory creation failure: " << bindir;
    return false;
  }

  // Exec path
  // Make a symlink with the name of appid, pointing exec file
  bf::path symlink_path = bindir / bf::path(app->appid);
  LOG(DEBUG) << "Creating symlink " << symlink_path << " pointing " <<
      app->exec;
  bf::create_symlink(bf::path(app->exec), symlink_path, boost_error);
  if (boost_error) {
    LOG(ERROR) << "Symlink creation failure: " << symlink_path;
    return false;
  }

  // Give an execution permission to the original executable
  LOG(DEBUG) << "Giving exec permission to " << app->exec;
  bf::permissions(bf::path(app->exec), bf::owner_all |
      bf::group_read | bf::group_exe |
      bf::others_read | bf::others_exe, boost_error);
  if (boost_error) {
    LOG(ERROR) << "Permission change failure";
    return false;
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
  if (!m->application) {
    LOG(ERROR) << "No application exists";
    return Step::Status::ERROR;
  }

  return Step::Status::OK;
}

Status StepCreateSymbolicLink::process() {
  manifest_x* m = context_->manifest_data.get();
  for (application_x* app : GListRange<application_x*>(m->application)) {
    if (!CreateSymLink(app, context_))
      return Status::ERROR;
  }
  return Status::OK;
}


Status StepCreateSymbolicLink::undo() {
  manifest_x* m = context_->manifest_data.get();
  Step::Status ret = Status::OK;
  for (application_x* app : GListRange<application_x*>(m->application)) {
    if (!CreateSymLink(app, context_))
      ret = Status::ERROR;
  }
  return ret;
}

}  // namespace filesystem
}  // namespace tpk
