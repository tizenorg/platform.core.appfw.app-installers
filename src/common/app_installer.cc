/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include <cstdio>

#include "common/app_installer.h"
#include "common/context_installer.h"
#include "common/pkgmgr_interface.h"
#include "common/pkgmgr_signal.h"
#include "utils/logging.h"

#define STR_EMPTY ""

namespace {

const unsigned kProgressRange = 100;

}

namespace common_installer {

AppInstaller::AppInstaller(const char* package_type)
    : context_(new ContextInstaller()) {
  PkgMgrPtr pkgmgr = PkgMgrInterface::Instance();
  pi_.reset(new PkgmgrSignal(pkgmgr.get()->GetRawPi()));
  context_->pkg_type.set(package_type);
  switch (pkgmgr->GetRequestType()) {
    case PkgMgrInterface::Type::Install:
      context_->file_path.set(pkgmgr->GetRequestInfo());
      context_->pkgid.set(STR_EMPTY);
      break;
    case PkgMgrInterface::Type::Uninstall:
      context_->pkgid.set(pkgmgr->GetRequestInfo());
      context_->file_path.set(STR_EMPTY);
      break;
    default:
      // currently, only installation and uninstallation handled
      // TODO(p.sikorski): should return unsupported, and display error
      break;
  }
}

AppInstaller::~AppInstaller() {
}

int AppInstaller::Run() {
  std::list<std::unique_ptr<Step>>::iterator it(steps_.begin());
  std::list<std::unique_ptr<Step>>::iterator itStart(steps_.begin());
  std::list<std::unique_ptr<Step>>::iterator itEnd(steps_.end());

  Step::Status process_status = Step::Status::OK;
  int ret = 0;
  unsigned total_steps = steps_.size();
  unsigned current_step = 1;

  for (; it != itEnd; ++it, ++current_step) {
    process_status = (*it)->process();

    // send START signal as soon as possible
    if (pi_->state() == PkgmgrSignal::State::NOT_SENT) {
      if (!context_->pkgid.get().empty()) {
        pi_->SendStarted(context_->pkg_type.get(), context_->pkgid.get());
      }
    }

    if (process_status != Step::Status::OK) {
      LOG(ERROR) << "Error during processing";
      ret = -1;
      break;
    }

    // send installation progress
    pi_->SendProgress(
        current_step * kProgressRange / total_steps,
        context_->pkg_type.get(), context_->pkgid.get());
  }

  if (it != itEnd) {
    LOG(ERROR) << "Failure occurs";
    do {
      if ((*it)->undo() != Step::Status::OK) {
        LOG(ERROR) << "Error during undo operation";
        ret = -2;
      }
    } while (it-- != itStart);
  } else {
    while (it-- != itStart) {
      if ((*it)->clean() != Step::Status::OK) {
        LOG(ERROR) << "Error during clean operation";
        ret = -3;
        break;
      }
    }
  }

  // send START if pkgid was not parsed
  if (pi_->state() == PkgmgrSignal::State::NOT_SENT) {
    pi_->SendStarted(context_->pkg_type.get(), context_->pkgid.get());
  }
  pi_->SendFinished(process_status,
                    context_->pkg_type.get(),
                    context_->pkgid.get());
  return ret;
}

}  // namespace common_installer
