/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include <cstdio>

#include "common/app_installer.h"
#include "common/installer_context.h"
#include "common/pkgmgr_interface.h"
#include "common/pkgmgr_signal.h"

namespace {

const unsigned kProgressRange = 100;

}

namespace common_installer {

AppInstaller::AppInstaller(const char* package_type, PkgMgrPtr pkgmgr)
    : pkgmgr_(pkgmgr),
      context_(new InstallerContext()) {
  pi_.reset(new PkgmgrSignal(pkgmgr.get()->GetRawPi(),
                             pkgmgr->GetRequestType()));

  // TODO(p.sikorski) below property is only used in AppInstaller.
  // maybe it should then be kept in AppInstaller
  context_->pkg_type.set(package_type);
}

AppInstaller::~AppInstaller() {
}

AppInstaller::Result AppInstaller::Run() {
  std::list<std::unique_ptr<Step>>::iterator it(steps_.begin());
  std::list<std::unique_ptr<Step>>::iterator itStart(steps_.begin());
  std::list<std::unique_ptr<Step>>::iterator itEnd(steps_.end());

  Step::Status process_status = Step::Status::OK;
  Result ret = Result::OK;
  unsigned total_steps = steps_.size();
  unsigned current_step = 1;

  for (; it != itEnd; ++it, ++current_step) {
    try {
      process_status = (*it)->precheck();
      if (process_status == Step::Status::OK) {
        process_status = (*it)->process();
      }
    } catch (const std::exception& err) {
      LOG(ERROR) << "Exception occurred in process(): " << err.what();
      process_status = Step::Status::ERROR;
    }

    // send START signal as soon as possible
    if (pi_->state() == PkgmgrSignal::State::NOT_SENT) {
      if (!context_->pkgid.get().empty()) {
        pi_->SendStarted(context_->pkg_type.get(), context_->pkgid.get());
      }
    }

    if (process_status != Step::Status::OK) {
      LOG(ERROR) << "Error during processing";
      ret = Result::ERROR;
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
      try {
        if ((*it)->undo() != Step::Status::OK) {
          LOG(ERROR) << "Error during undo operation, but continuing...";
          ret = Result::UNDO_ERROR;
        }
      } catch (const std::exception& err) {
        LOG(ERROR) << "Exception occurred in undo(): " << err.what();
        ret = Result::UNDO_ERROR;
      }
    } while (it-- != itStart);
  } else {
    for (auto& step : steps_) {
      try {
        if (step->clean() != Step::Status::OK) {
          LOG(ERROR) << "Error during clean operation";
          ret = Result::CLEANUP_ERROR;
          break;
        }
      } catch (const std::exception& err) {
        LOG(ERROR) << "Exception occurred in clean(): " << err.what();
        ret = Result::CLEANUP_ERROR;
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

void AppInstaller::HandleStepError(Step::Status result, const std::string& error) {
  pi_->SendError(result, error, context_->pkg_type.get(), context_->pkgid.get());
}

}  // namespace common_installer
