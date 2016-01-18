// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/plugins_launcher.h"

#include <dlfcn.h>

#include <map>
#include <utility>

namespace common_installer {

bool PluginsLauncher::LaunchPlugin(const std::string& plugin, xmlDocPtr docPtr,
                                   ActionType action, const std::string& pkgId,
                                   int* result) {
  LOG(DEBUG) << "Installing plugin: " << plugin;

  std::unique_ptr<DynamicLibHandle> dl_handle =
      DynamicLibHandle::Create(GetPath(plugin), RTLD_LAZY | RTLD_LOCAL);
  if (!dl_handle) {
    LOG(ERROR) << "Failed to create library handle";
    return false;
  }

  if (!RunPluginFunction(dl_handle.get(), ProcessType::PreInstall, action,
                         pkgId.c_str(), result)) {
    LOG(ERROR) << "Preinstall failed";
    return false;
  }

  if (!RunPluginFunction(dl_handle.get(), ProcessType::Install, action,
                         docPtr, pkgId.c_str(), result)) {
    LOG(ERROR) << "Install failed";
    return false;
  }
  if (!RunPluginFunction(dl_handle.get(), ProcessType::PostInstall, action,
                         pkgId.c_str(), result)) {
    LOG(ERROR) << "Postinstall failed";
    return false;
  }
  return true;
}

std::string PluginsLauncher::GetPath(const std::string& plugin) {
  return "/usr/etc/package-manager/parser/lib/lib" + plugin + ".so";
}

bool PluginsLauncher::GetName(ProcessType process, ActionType action,
                              std::string* result) {
  static std::map<std::pair<ActionType, ProcessType>, std::string> names {
    {{ActionType::Install,   ProcessType::PreInstall},  "PKGMGR_PARSER_PLUGIN_PRE_INSTALL"},  // NOLINT
    {{ActionType::Upgrade,   ProcessType::PreInstall},  "PKGMGR_PARSER_PLUGIN_PRE_UPGRADE"},  // NOLINT
    {{ActionType::Uninstall, ProcessType::PreInstall},  "PKGMGR_PARSER_PLUGIN_PRE_UNINSTALL"},  // NOLINT
    {{ActionType::Install,   ProcessType::Install},     "PKGMGR_PARSER_PLUGIN_INSTALL"},  // NOLINT
    {{ActionType::Upgrade,   ProcessType::Install},     "PKGMGR_PARSER_PLUGIN_UPGRADE"},  // NOLINT
    {{ActionType::Uninstall, ProcessType::Install},     "PKGMGR_PARSER_PLUGIN_UNINSTALL"},  // NOLINT
    {{ActionType::Install,   ProcessType::PostInstall}, "PKGMGR_PARSER_PLUGIN_POST_INSTALL"},  // NOLINT
    {{ActionType::Upgrade,   ProcessType::PostInstall}, "PKGMGR_PARSER_PLUGIN_POST_UPGRADE"},  // NOLINT
    {{ActionType::Uninstall, ProcessType::PostInstall}, "PKGMGR_PARSER_PLUGIN_POST_UNINSTALL"}  // NOLINT
  };

  auto pos = names.find(std::make_pair(action, process));
  if (pos == names.end()) {
    LOG(ERROR) << "Function name not defined";
    return false;
  }
  *result = pos->second;
  return true;
}

bool PluginsLauncher::RunPluginFunction(DynamicLibHandle* dl_handle,
                                        ProcessType process, ActionType action,
                                        const char* pkgId, int* result) {
  return RunPluginFunctionImpl(dl_handle, process, action, result, pkgId);
}

bool PluginsLauncher::RunPluginFunction(DynamicLibHandle* dl_handle,
                                        ProcessType process, ActionType action,
                                        xmlDocPtr docPtr, const char* pkgId,
                                        int* result) {
  return RunPluginFunctionImpl(dl_handle, process, action, result, docPtr,
                               pkgId);
}

}  // namespace common_installer
