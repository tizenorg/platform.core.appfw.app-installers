// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/pkgmgr/step_kill_apps.h"

#include <sys/time.h>
#include <string>
#include <systemd/sd-login.h>

#include "aul.h"
#include "common/utils/glist_range.h"

namespace {

bool KillApp(const std::string& appid) {
  uid_t *uids = NULL;
  uid_t uid = -1;
  int ret = -1;
  int pid = 0;
  int i;
  char *state;

  uid = getuid();
  if (uid == 0 || uid == tzplatform_getuid(TZ_SYS_GLOBALAPP_USER)) {
    ret = sd_get_uids(&uids);
    if (ret < 0 || (ret == 0 || uids == NULL)) {
      LOG(ERROR) << "Failed to get uids [" << ret << "]";
      return false;
    }

    for (i = 0; i < ret; i++) {
      if (sd_uid_get_state(uids[i], &state) < 0) {
        LOG(ERROR) << "Failed to get state of uid " << uids[i];
        free(uids);
        return false;
      } else {
        if (!strncmp(state, "online", 6)) {
          uid = uids[i];
          break;
        }
        LOG(ERROR) << "state of " << uids[i] << " is " << state;
      }
    }
    free(uids);
    free(state);

    if (uid == -1) {
      LOG(ERROR) << "Failed to get online uid";
      return false;
    }
  }

  ret = aul_app_is_running_for_uid(appid.c_str(), uid);
  if (ret == 0)
    return false;

  pid = aul_app_get_pid_for_uid(appid.c_str(), uid);
  if (pid < 0) {
    LOG(ERROR) << "Failed to get pid for appid : " << appid;
    return false;
  }

  ret = aul_terminate_pid_sync_for_uid(pid, uid);
  if (ret != AUL_R_OK) {
    LOG(ERROR) << "Failed to kill app : " << appid;
    return false;
  }

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
