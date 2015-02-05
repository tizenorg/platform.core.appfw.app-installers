/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "common/step/step_copy.h"

#include <cassert>
#include <cstring>
#include <string>

#include "common/utils.h"

namespace common_installer {
namespace copy {

namespace fs = boost::filesystem;

Step::Status StepCopy::process() {
  assert(!context_->pkgid().empty());

  fs::path install_path = fs::path(context_->GetApplicationPath());


  context_->set_pkg_path(install_path.string());

  // FIXME: correctly order app's data.
  // If there is 1 app in package, app's data are stored in <pkg_path>/<app_id>
  // If there are >1 apps in package, app's data are stored in <pkg_path>
  // considering that multiple apps data are already separated in folders.
  if (context_->manifest_data()->uiapplication &&
      !context_->manifest_data()->uiapplication->next)
    install_path /= fs::path(context_->manifest_data()->mainapp_id);

  if (!utils::CopyDir(fs::path(context_->unpack_directory()), install_path)) {
    LOG(ERROR) << "Fail to copy tmp dir: " << context_->unpack_directory()
               << " to dst dir: " << install_path.string();
    return Step::Status::ERROR;
  }

  LOG(INFO) << "Successfully copy: " << context_->unpack_directory()
            << " to: " << install_path.string() << " directory";
  return Status::OK;
}

Step::Status StepCopy::clean() {
  LOG(DEBUG) << "Remove tmp dir: " << context_->unpack_directory();
  fs::remove_all(context_->unpack_directory());
  return Status::OK;
}

Step::Status StepCopy::undo() {
  if (fs::exists(context_->pkg_path()))
    fs::remove_all(context_->pkg_path());
  return Status::OK;
}

}  // namespace copy
}  // namespace common_installer
