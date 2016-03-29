/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_APP_INSTALLER_H_
#define COMMON_APP_INSTALLER_H_

#include <manifest_parser/utils/logging.h>
#include <boost/bind.hpp>
#include <list>
#include <memory>
#include <string>

#include "common/pkgmgr_interface.h"
#include "common/pkgmgr_signal.h"
#include "common/step/step.h"
#include "common/utils/macros.h"

namespace common_installer {

/**
 * \brief Includes the main “steps” processing mechanisms. It holds
 *        the lists of steps and runs each of the step in the configured
 *        order.
 */
__attribute__ ((visibility ("default"))) class AppInstaller {
 public:
  /** Enumeration of possible returned Results */
  enum class Result {
    OK,
    ERROR,
    CLEANUP_ERROR,
    UNDO_ERROR,
    UNKNOWN
  };

  /**
   * \brief explicit Constructor
   *
   * \param package_type package type
   * \param pkgmgr pointer to PkgMgrInterface object
   */
  explicit AppInstaller(const char* package_type, PkgMgrPtr pkgmgr);

  /** virtual desctructor */
  virtual ~AppInstaller();

  /**
   * \brief Adds new step to installer by specified type
   *        Type of template parameter is used to create requested step
   *        class instance.
   *        Context of installer is passed to step in this method
   *        and is not being exposed outside installer.
   *        Step arguments are deduced and forwarded to constructor.
   *
   * \tparam StepT Step object to be added to the step list
   * \tparam Args constructor arguments for StepT object
   * \param args argument list
   */
  template<class StepT, class... Args>
  void AddStep(Args&&... args) {
    std::unique_ptr<Step> step(
        new StepT(context_.get(), std::forward<Args>(args)...));
    step->on_error.connect(
        boost::bind(&AppInstaller::HandleStepError, this, _1, _2));
    steps_.emplace_back(std::move(step));
  }

  /**
   * \brief runs the steps in the specific sequence
   *
   * \return Result of the run (eg Result:OK)
   */
  Result Run();

 protected:
  PkgMgrPtr pkgmgr_;
  std::unique_ptr<InstallerContext> context_;

 private:
  std::list<std::unique_ptr<Step>> steps_;

  // data used to send signal
  std::unique_ptr<PkgmgrSignal> pi_;

  void HandleStepError(Step::Status result, const std::string& error);

  SCOPE_LOG_TAG(AppInstaller)

  DISALLOW_COPY_AND_ASSIGN(AppInstaller);
};

}  // namespace common_installer

#endif  // COMMON_APP_INSTALLER_H_
