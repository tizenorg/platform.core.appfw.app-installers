// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_PKGMGR_SIGNAL_H_
#define COMMON_PKGMGR_SIGNAL_H_

#include <pkgmgr_installer.h>

#include <memory>
#include <string>

#include "common/request.h"
#include "common/step/step.h"
#include "common/utils/macros.h"

namespace common_installer {

/**
 * \brief Adapter interface for pkgmgr for sending notification/progress
 *        to pkgmgr.
 *
 * Signals are sent using API defined and exposed by pkgmgr (external) module.
 * Pkgmgr is notified about:
 * * installation start,
 * * installation result,
 * * installation end,
 * * installation progress.
 */
class PkgmgrSignal {
 public:
  enum class State {
    NOT_SENT,
    STARTED,
    FINISHED
  };

  /**
   * Constructor
   *
   * \param pi pointer for pkgmgr_installer structure
   * \param req_type request type
   */
  explicit PkgmgrSignal(pkgmgr_installer* pi, RequestType req_type);

  /**
   * "start" Signal sending
   *
   * \param type package type
   * \param pkgid package pkgid
   *
   * \return true if success
   */
  bool SendStarted(
      const std::string& type = std::string(),
      const std::string& pkgid = std::string());

  /**
   * "progress" Signal sending
   *
   * \param type package type
   * \param pkgid package pkgid
   *
   * \return true if success
   */
  bool SendProgress(int progress,
      const std::string& type = std::string(),
      const std::string& pkgid = std::string());

  /**
   * "finished" Signal sending
   *
   * \param type package type
   * \param pkgid package pkgid
   *
   * \return true if success
   */
  bool SendFinished(
      Step::Status result,
      const std::string& type = std::string(),
      const std::string& pkgid = std::string());

  /**
   * "error" Signal sending
   *
   * \param error_message error message content
   *
   * \return true if success
   */
  bool SendError(const std::string& error_message,
      const std::string& type = std::string(),
      const std::string& pkgid = std::string());

  bool IsFinished() const;

  /**
   * Getter of state of the object
   *
   *\return current state
   */
  State state() const { return state_; }

 private:
  bool SendSignal(
      const char* key,
      const char* value,
      const std::string& type = std::string(),
      const std::string& pkgid = std::string()) const;
  const char* GetResultKey(Step::Status result) const;
  bool SendAppids(const std::string& type, const std::string& pkgid) const;

  pkgmgr_installer* pi_;
  static State state_;
  RequestType request_type_;
  bool error_message_sent_;

  DISALLOW_COPY_AND_ASSIGN(PkgmgrSignal);
};

}  // namespace common_installer

#endif  // COMMON_PKGMGR_SIGNAL_H_
