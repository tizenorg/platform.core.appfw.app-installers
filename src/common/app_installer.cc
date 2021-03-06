/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include <cstdio>
#include <fstream>
#include <unistd.h>

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
  context_->pkg_type.set(package_type);
  context_->installation_mode.set(pkgmgr->GetInstallationMode());

  // pkgmgr signal should work only for online mode
  // there is no one to receive it in offline mode
  if (context_->installation_mode.get() == InstallationMode::ONLINE) {
    pi_ = pkgmgr->CreatePkgmgrSignal();
  }
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
    } catch (const std::exception& err) {
      LOG(ERROR) << "Exception occurred in precheck(): " << err.what()
                 << " in step: " << (*it)->name();
      process_status = Step::Status::ERROR;
    }
    try {
      if (process_status == Step::Status::OK) {
        process_status = (*it)->process();
      }
    } catch (const std::exception& err) {
      LOG(ERROR) << "Exception occurred in process(): " << err.what()
                 << " in step: " << (*it)->name();
      process_status = Step::Status::ERROR;
    }

    if (process_status != Step::Status::OK) {
      LOG(ERROR) << "Error during processing";
      ret = Result::ERROR;
      break;
    }

    if (pi_) {
      // send START signal as soon as possible if not sent
      if (pi_->state() == PkgmgrSignal::State::NOT_SENT) {
        if (!context_->pkgid.get().empty()) {
          pi_->SendStarted(context_->pkg_type.get(), context_->pkgid.get());
        }
      }

      // send installation progress
      pi_->SendProgress(
          current_step * kProgressRange / total_steps,
          context_->pkg_type.get(), context_->pkgid.get());
    }
  }

  if (it != itEnd) {
    LOG(ERROR) << "Failure occurs in step: " << (*it)->name();
    do {
      try {
        if ((*it)->undo() != Step::Status::OK) {
          LOG(ERROR) << "Error during undo operation(" << (*it)->name()
                     << "), but continuing...";
          ret = Result::UNDO_ERROR;
        }
      } catch (const std::exception& err) {
        LOG(ERROR) << "Exception occurred in undo(): " << err.what()
                   << " in step: " << (*it)->name();
        ret = Result::UNDO_ERROR;
      }
    } while (it-- != itStart);
  } else {
    for (auto& step : steps_) {
      try {
        if (step->clean() != Step::Status::OK) {
          LOG(ERROR) << "Error during clean operation(" << step->name() << ")";
          ret = Result::CLEANUP_ERROR;
          break;
        }
      } catch (const std::exception& err) {
        LOG(ERROR) << "Exception occurred in clean(): " << err.what()
                   << " in step: " << step->name();
        ret = Result::CLEANUP_ERROR;
        break;
      }
    }
  }
  sync();

  if (pi_) {
    // send START if pkgid was not parsed
    if (pi_->state() == PkgmgrSignal::State::NOT_SENT) {
      pi_->SendStarted(context_->pkg_type.get(), context_->pkgid.get());
    }
    pi_->SendFinished(process_status,
                      context_->pkg_type.get(),
                      context_->pkgid.get());
  }

  if (context_->installation_mode.get() == InstallationMode::OFFLINE &&
      context_->is_preload_request.get() &&
      process_status != Step::Status::OK) {
    std::fstream info_file("/tmp/.preload_install_error",
        std::ios::out | std::ios::app);
    info_file << context_->pkgid.get() << std::endl;
    info_file.close();
  }

  return ret;
}

void AppInstaller::HandleStepError(Step::Status result,
                                        const std::string& error) {
  if (pi_)
    pi_->SendError(result, error, context_->pkg_type.get(),
                   context_->pkgid.get());
}

}  // namespace common_installer
