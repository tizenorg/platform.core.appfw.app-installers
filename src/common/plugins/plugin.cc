// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/plugins/plugin.h"

#include <pkgmgr_parser.h>

#include <memory>

namespace common_installer {

Plugin::Plugin(const PluginInfo& plugin_info)
    : plugin_info_(plugin_info),
      lib_handle_(nullptr) {}

bool Plugin::Load() {
  if (lib_handle_) {
    return true;
  }

  lib_handle_ = dlopen(plugin_info_.path().c_str(), RTLD_LAZY | RTLD_LOCAL);
  if (!lib_handle_) {
    LOG(WARNING) << "Failed to open library: " << plugin_info_.path().c_str()
               << " (" << dlerror() << ")";
    return false;
  }
  return true;
}

void* Plugin::GetSymbol(const std::string& name) const {
  return dlsym(lib_handle_, name.c_str());
}

Plugin::~Plugin() {
  if (lib_handle_) {
    dlclose(lib_handle_);
  }
}

int ActionTypeToPkgmgrActionType(common_installer::Plugin::ActionType action) {
  switch (action) {
    case Plugin::ActionType::Install:
      return ACTION_INSTALL;
    case Plugin::ActionType::Upgrade:
      return ACTION_UPGRADE;
    case Plugin::ActionType::Uninstall:
      return ACTION_UNINSTALL;
    default:
      LOG(ERROR) << "Failed to get correct action type";
      return -1;
  }
}

}  // namespace common_installer
