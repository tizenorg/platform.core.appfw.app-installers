/* 2014, Copyright © Eurogiciel Coporation, APACHE-2.0, see LICENSE file */

#include "common/step/step_signal.h"

#include <iostream>

#include "common/utils.h"

#define DBG(msg) std::cout << "[Signal] " << msg << std::endl;
#define ERR(msg) std::cout << "[ERROR: Signal] " << msg << std::endl;

namespace common_installer {
namespace signal {

Step::Status StepSignal::process() {
  if (!context_->pi()->sendStarted(
      context_->manifest_data()->type, context_->pkgid())) {
    return Status::ERROR;
  }
  DBG("Send Start");
  return Status::OK;
}

Step::Status StepSignal::clean() {
  if (!context_->pi()->sendFinished(
        PkgmgrSignal::Result::SUCCESS,
        context_->manifest_data()->type, context_->pkgid())) {
    return Status::ERROR;
  }
  DBG("Send Success");
  return Status::OK;
}

Step::Status StepSignal::undo() {
  if (!context_->pi()->sendFinished(
        PkgmgrSignal::Result::FAILED,
        context_->manifest_data()->type, context_->pkgid())) {
    return Status::ERROR;
  }
  DBG("Send Error");
  return Status::OK;
}

}  // namespace signal
}  // namespace common_installer
