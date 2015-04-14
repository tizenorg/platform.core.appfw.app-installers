/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "common/step/step_copy.h"

#include <cassert>
#include <cstring>
#include <string>

#include "utils/file_util.h"

namespace common_installer {
namespace copy {

namespace bf = boost::filesystem;
namespace bs = boost::system;

Step::Status StepCopy::process() {
  assert(!context_->pkgid.get().empty());

  // set application path
  context_->application_path.set(
      context_->root_application_path.get() / context_->pkgid.get());

  bf::path install_path = bf::path(context_->application_path.get());

  context_->pkg_path.set(install_path.string());

  // FIXME: correctly order app's data.
  // If there is 1 app in package, app's data are stored in <pkg_path>/<app_id>
  // If there are >1 apps in package, app's data are stored in <pkg_path>
  // considering that multiple apps data are already separated in folders.
  if (context_->manifest_data.get()->uiapplication &&
      !context_->manifest_data.get()->uiapplication->next)
    install_path /= bf::path(context_->manifest_data.get()->mainapp_id);

  bs::error_code error;
  bf::create_directories(install_path.parent_path(), error);
  if (error) {
    LOG(ERROR) << "Cannot create directory: "
               << install_path.parent_path().string();
    return Step::Status::ERROR;
  }
  bf::rename(context_->unpacked_dir_path.get(), install_path, error);
  if (error) {
    LOG(DEBUG) << "Cannot move directory. Will try to copy...";
    if (!utils::CopyDir(bf::path(context_->unpacked_dir_path.get()),
        install_path)) {
      LOG(ERROR) << "Fail to copy tmp dir: "
          << context_->unpacked_dir_path.get()
          << " to dst dir: " << install_path.string();
      return Step::Status::ERROR;
    }
    bs::error_code error;
    bf::remove_all(context_->unpacked_dir_path.get(), error);
    if (error) {
      LOG(WARNING) << "Cannot remove temporary directory: "
                   << context_->unpacked_dir_path.get();
    }
  }
  LOG(INFO) << "Successfully move/copy: " << context_->unpacked_dir_path.get()
            << " to: " << install_path.string() << " directory";
  return Status::OK;
}

Step::Status StepCopy::clean() {
  return Status::OK;
}

Step::Status StepCopy::undo() {
  if (bf::exists(context_->pkg_path.get()))
    bf::remove_all(context_->pkg_path.get());
  return Status::OK;
}

}  // namespace copy
}  // namespace common_installer
