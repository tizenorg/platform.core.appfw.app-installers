// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/pkgmgr_signal.h"

#include <cassert>

#include "utils/logging.h"

namespace common_installer {

PkgmgrSignal::PkgmgrSignal(pkgmgr_installer* pi)
    : pi_(pi),
      state_(State::NOT_SENT) {
}

bool PkgmgrSignal::sendStarted(
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

bool PkgmgrSignal::sendFinished(
    Result result, const std::string& type, const std::string& pkgid) {
  if (state_ != State::STARTED) {
    return false;
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

const char* PkgmgrSignal::GetResultKey(Result result) const {
  switch (result) {
    case Result::SUCCESS:
      return PKGMGR_INSTALLER_OK_EVENT_STR;
    case Result::FAILED:
      return PKGMGR_INSTALLER_FAIL_EVENT_STR;
    default:
      assert(false && "Not Reached");
  }
}

}  // namespace common_installer
