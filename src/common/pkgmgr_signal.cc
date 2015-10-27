// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/pkgmgr_signal.h"

#include <unistd.h>
#include <sys/types.h>

#include <cassert>
#include <map>
#include <vector>

#include "common/pkgmgr_registration.h"
#include "common/utils/logging.h"

namespace {

namespace ci = common_installer;

const std::map<ci::RequestType, const char*> kEventStr = {
  {ci::RequestType::Install, PKGMGR_INSTALLER_INSTALL_EVENT_STR},
  {ci::RequestType::Recovery, PKGMGR_INSTALLER_INSTALL_EVENT_STR},
  {ci::RequestType::Reinstall, PKGMGR_INSTALLER_INSTALL_EVENT_STR},
  {ci::RequestType::Uninstall, PKGMGR_INSTALLER_UNINSTALL_EVENT_STR},
  {ci::RequestType::Update, PKGMGR_INSTALLER_UPGRADE_EVENT_STR},
  {ci::RequestType::Delta, PKGMGR_INSTALLER_UPGRADE_EVENT_STR}
};

}  // namespace

namespace common_installer {

PkgmgrSignal::State PkgmgrSignal::state_ = PkgmgrSignal::State::NOT_SENT;

PkgmgrSignal::PkgmgrSignal(pkgmgr_installer* pi, RequestType req_type)
    : pi_(pi), request_type_(req_type) {
}

bool PkgmgrSignal::SendStarted(
    const std::string& type, const std::string& pkgid) {
  if (state_ != State::NOT_SENT) {
    return false;
  }

  auto key = kEventStr.find(request_type_);
  if (key == kEventStr.end()) {
    return false;
  }
  if (!SendSignal(PKGMGR_INSTALLER_START_KEY_STR, key->second, type, pkgid)) {
    return false;
  }

  state_ = State::STARTED;

  // workaround for pkgmgr client to know all appids which are uninstalled
  if (request_type_ == ci::RequestType::Uninstall)
    if (!SendAppids(type, pkgid))
      return false;

  return true;
}

bool PkgmgrSignal::SendProgress(int progress,
    const std::string& type, const std::string& pkgid) {
  if (state_ != State::STARTED) {
    return false;
  }

  return SendSignal(PKGMGR_INSTALLER_INSTALL_PERCENT_KEY_STR,
      std::to_string(progress).c_str(), type, pkgid);
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

bool PkgmgrSignal::SendAppids(const std::string& type,
                              const std::string& pkgid) const {
  std::vector<std::string> appids;
  if (!QueryAppidsForPkgId(pkgid, &appids, getuid()))
    return true;
  for (auto& appid : appids) {
    if (!pkgmgr_installer_send_app_uninstall_signal(pi_, type.c_str(),
                                                    pkgid.c_str(),
                                                    appid.c_str()))
      return false;
  }
  return true;
}

}  // namespace common_installer
