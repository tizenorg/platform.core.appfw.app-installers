/* 2014, Copyright © Eurogiciel Coporation, APACHE-2.0, see LICENSE file */

#include "common/step/step_signal.h"

#include <pkgmgr-info.h>
#include <unistd.h>

#include "common/utils.h"

namespace common_installer {
namespace signal {

Step::Status StepSignal::process() {
  if (!context_->pi()->sendStarted(
      context_->manifest_data()->type, context_->pkgid())) {
    return Status::ERROR;
  }
  LOG(DEBUG) << "Send Start";
  return Status::OK;
}

Step::Status StepSignal::clean() {
  if (!context_->pi()->sendFinished(
        PkgmgrSignal::Result::SUCCESS,
        context_->manifest_data()->type, context_->pkgid())) {
    return Status::ERROR;
  }
  LOG(DEBUG) << "Send Success";
  return Status::OK;
}

Step::Status StepSignal::undo() {
  if (!context_->pi()->sendFinished(
        PkgmgrSignal::Result::FAILED,
        context_->manifest_data()->type, context_->pkgid())) {
    return Status::ERROR;
  }
  LOG(ERROR) << "Send Error";
  return Status::OK;
}

}  // namespace signal
}  // namespace common_installer
