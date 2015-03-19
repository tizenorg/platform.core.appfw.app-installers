// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/pkgmgr_signal.h"

#include <cassert>

#include "utils/logging.h"

// Redefine this value as it is not exported by pkgmgr
// this should probably be in public interface because of
// otherwise there is no way to return errorcode
#define PKGMGR_INSTALLER_ERROR_KEY_STR "error"

namespace common_installer {

PkgmgrSignal::PkgmgrSignal(pkgmgr_installer* pi)
    : pi_(pi),
      state_(State::NOT_SENT) {
}

bool PkgmgrSignal::SendStarted(
    const std::string& type, const std::string& pkgid) {
  if (state_ != State::NOT_SENT) {
    return false;
  }

  if (!SendSignal(PKGMGR_INSTALLER_START_KEY_STR,
      (pkgmgr_installer_get_request_type(pi_) != PKGMGR_REQ_UNINSTALL)
      ? PKGMGR_INSTALLER_INSTALL_EVENT_STR
      : PKGMGR_INSTALLER_UNINSTALL_EVENT_STR,
      type, pkgid)) {
    return false;
  }
  state_ = State::STARTED;
  return true;
}

bool PkgmgrSignal::SendFinished(
    Step::Status result, const std::string& type, const std::string& pkgid) {
  if (state_ != State::STARTED) {
    return false;
  }
  if (result != Step::Status::OK) {
    if (!SendSignal(
        PKGMGR_INSTALLER_ERROR_KEY_STR,
        std::to_string(static_cast<int>(result)).c_str(), type, pkgid)) {
      return false;
    }
  }
  if (!SendSignal(
      PKGMGR_INSTALLER_END_KEY_STR, GetResultKey(result), type, pkgid)) {
    return false;
  }
  state_ = State::FINISHED;
  return true;
}

bool PkgmgrSignal::IsFinished() const {
  return state_ == State::FINISHED;
}

bool PkgmgrSignal::SendSignal(
    const char* key,
    const char* value,
    const std::string& type,
    const std::string& pkgid) const {
  // send pkgmgr signal
  if (pkgmgr_installer_send_signal(
        pi_,
        !type.empty() ?  type.c_str(): "",
        !pkgid.empty() ? pkgid.c_str() : "",
        key,
        value)) {
    LOG(ERROR) << "Fail to send pkgmgr signal";
    return false;
  }

  LOG(DEBUG) << "Success to send pkgmgr signal"
             << " PKGID=" << pkgid
             << " KEY=" << key
             << " VALUE=" << value;
  return true;
}

const char* PkgmgrSignal::GetResultKey(Step::Status result) const {
  switch (result) {
    case Step::Status::OK:
      return PKGMGR_INSTALLER_OK_EVENT_STR;
    default:
      return PKGMGR_INSTALLER_FAIL_EVENT_STR;
  }
}

}  // namespace common_installer
