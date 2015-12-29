// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_kill_apps.h"

#include <app_manager.h>
#include <app_manager_extension.h>

#include <string>

#include "common/utils/glist_range.h"

namespace {

bool KillApp(const std::string& appid) {
  bool is_running = false;
  if (app_manager_is_running(appid.c_str(), &is_running)
      != APP_MANAGER_ERROR_NONE) {
    LOG(ERROR) << "app_manager_is_running failed";
    return false;
  }
  if (!is_running) {
    return false;
  }
  app_context_h app_context;
  if (app_manager_get_app_context(appid.c_str(), &app_context)
      != APP_MANAGER_ERROR_NONE) {
    LOG(ERROR) << "app_manager_get_app_context failed";
    return false;
  }
  if (app_manager_terminate_app(app_context)
      != APP_MANAGER_ERROR_NONE) {
    LOG(ERROR) << "app_manager_terminate_app failed";
    app_context_destroy(app_context);
    return false;
  }
  LOG(DEBUG) << "Application '" << appid << "' has been killed";
  app_context_destroy(app_context);
  return true;
}

}  // namespace

namespace common_installer {
namespace pkgmgr {

Step::Status StepKillApps::process() {
  ManifestXWrapperPtr old_manifest = context_->old_manifest_data.get() ?
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
