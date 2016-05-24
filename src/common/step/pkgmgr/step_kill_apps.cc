// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/pkgmgr/step_kill_apps.h"

#include <sys/time.h>
#include <systemd/sd-login.h>
#include <aul.h>

#include <string>

#include "common/utils/glist_range.h"

namespace {

bool CheckAndKill(const std::string& appid, uid_t uid) {
  int ret = aul_app_is_running_for_uid(appid.c_str(), uid);
  if (ret == 0)
    return true;

  int pid = aul_app_get_pid_for_uid(appid.c_str(), uid);
  if (pid < 0) {
    LOG(ERROR) << "Failed to get pid for appid[" << appid
               << "] for uid [" << uid << "]";
    return true;
  }

  ret = aul_terminate_pid_sync_for_uid(pid, uid);
  if (ret != AUL_R_OK) {
    LOG(ERROR) << "Failed to kill app[" << appid << "] for uid [" << uid << "]";
    return false;
  }

  return true;
}

bool KillApp(const std::string& appid) {
  uid_t* uids = nullptr;
  int ret = -1;
  int i;

  uid_t uid = getuid();
  if (uid == 0 || uid == tzplatform_getuid(TZ_SYS_GLOBALAPP_USER)) {
    ret = sd_get_uids(&uids);
    if (ret < 0 || (ret == 0 || uids == nullptr)) {
      LOG(ERROR) << "Failed to get uids [" << ret << "]";
      return false;
    }

    for (i = 0; i < ret; i++) {
      if (!CheckAndKill(appid, uids[i])) {
        free(uids);
        return false;
      }
    }
  } else {
    if (!CheckAndKill(appid, uid))
      return false;
  }
  free(uids);
  LOG(DEBUG) << "Application '" << appid << "' has been killed";
  return true;
}

}  // namespace

namespace common_installer {
namespace pkgmgr {

Step::Status StepKillApps::process() {
  manifest_x* old_manifest = context_->old_manifest_data.get() ?
      context_->old_manifest_data.get() : context_->manifest_data.get();
  for (application_x* app :
       GListRange<application_x*>(old_manifest->application)) {
    (void) KillApp(app->appid);
  }
  return Status::OK;
}

Step::Status StepKillApps::precheck() {
  if (!context_->manifest_data.get() && !context_->old_manifest_data.get()) {
    LOG(ERROR) << "manifest_data is empty";
    return Status::ERROR;
  }
  return Status::OK;
}

}  // namespace pkgmgr
}  // namespace common_installer
