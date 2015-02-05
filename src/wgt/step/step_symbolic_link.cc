/* 2014, Copyright © Eurogiciel Coporation, APACHE-2.0, see LICENSE file */

#include "wgt/step/step_symbolic_link.h"

#include <pkgmgr-info.h>
#include <unistd.h>

#include <boost/filesystem.hpp>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <string>

#include "common/utils.h"

namespace wgt {
namespace symbolic_link {

namespace fs = boost::filesystem;

common_installer::Step::Status StepSymbolicLink::process() {
  assert(context_->manifest_data());
  boost::system::error_code error;
  uiapplication_x* ui = context_->manifest_data()->uiapplication;
  serviceapplication_x* svc = context_->manifest_data()->serviceapplication;
  if ((!ui) && (!svc)) {
     LOG(ERROR) << "There is neither UI applications nor"
        << "Services applications described!";
    return Step::Status::ERROR;
  }
  // add ui-application element per ui application
  for (; ui != nullptr; ui = ui->next) {
    // binary is a symbolic link named <appid> and is located in <pkgid>/<appid>
    fs::path exec_path = fs::path(context_->pkg_path()) / fs::path(ui->appid)
        / fs::path("bin");
    utils::CreateDir(exec_path);

    exec_path /= fs::path(ui->appid);

    fs::create_symlink(fs::path(WRT_LAUNCHER), exec_path, error);
    if (error) {
      LOG(ERROR) << "Failed to set symbolic link "
        << boost::system::system_error(error).what();
      return Step::Status::ERROR;
    }
  }
  for (; svc != nullptr; svc = svc->next) {
    // binary is a symbolic link named <appid> and is located in <pkgid>/<appid>
    fs::path exec_path = fs::path(context_->pkg_path()) / fs::path(svc->appid)
        / fs::path("bin");
    utils::CreateDir(exec_path);

    exec_path /= fs::path(svc->appid);

    fs::create_symlink(fs::path(WRT_LAUNCHER), exec_path, error);
    if (error) {
      LOG(ERROR) << "Failed to set symbolic link "
        << boost::system::system_error(error).what();
      return Step::Status::ERROR;
    }
  }
  LOG(DEBUG) << "Successfully parse tizen manifest xml";

  return Status::OK;
}

common_installer::Step::Status StepSymbolicLink::clean() {
  return Status::OK;
}

common_installer::Step::Status StepSymbolicLink::undo() {
  uiapplication_x* ui = context_->manifest_data()->uiapplication;
  serviceapplication_x* svc = context_->manifest_data()->serviceapplication;

  for (; ui != nullptr; ui = ui->next) {
    fs::path exec_path = fs::path(context_->pkg_path()) / fs::path(ui->appid)
    / fs::path("bin");
    if (fs::exists(exec_path))
      fs::remove_all(exec_path);
  }
  for (; svc != nullptr; svc = svc->next) {
    fs::path exec_path = fs::path(context_->pkg_path()) / fs::path(svc->appid)
    / fs::path("bin");
    if (fs::exists(exec_path))
      fs::remove_all(exec_path);
  }
  return Status::OK;
}

}  // namespace symbolic_link
}  // namespace wgt
