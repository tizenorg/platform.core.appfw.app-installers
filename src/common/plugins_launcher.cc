// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/plugins_launcher.h"

#include <dlfcn.h>

#include <map>
#include <utility>

namespace common_installer {

PluginsLauncher::Error PluginsLauncher::LaunchPlugin(
    const boost::filesystem::path& plugin_path, xmlDocPtr docPtr,
    ActionType action, const std::string& pkgId, int* result) {
  LOG(DEBUG) << "Installing plugin: " << plugin_path;

  DynamicLibHandle dlh;

  if (!dlh.Create(plugin_path, RTLD_LAZY | RTLD_LOCAL)) {
    LOG(ERROR) << "Failed to create library handle";
    return Error::FailedLibHandle;
  }

  ProcessType process_result = ProcessType::NoProcess;

  if (ExecPlugin(dlh, ProcessType::PreInstall, action, pkgId.c_str(),
                  result)) {
    process_result |= ProcessType::PreInstall;
  }

  if (ExecPlugin(dlh, ProcessType::Install, action, docPtr, pkgId.c_str(),
                  result)) {
    process_result |= ProcessType::Install;
  }

  if (ExecPlugin(dlh, ProcessType::PostInstall, action, pkgId.c_str(),
                  result)) {
    process_result |= ProcessType::PostInstall;
  }

  if (process_result == ProcessType::NoProcess) {
    return Error::ActionNotSupported;
  }

  return Error::Success;
}

bool PluginsLauncher::FunctionName(ProcessType process, ActionType action,
                              std::string* result) {
  static std::map<std::pair<ActionType, ProcessType>, std::string> names {
    {{ActionType::Install,   ProcessType::PreInstall},  "PKGMGR_PARSER_PLUGIN_PRE_INSTALL"},   // NOLINT
    {{ActionType::Upgrade,   ProcessType::PreInstall},  "PKGMGR_PARSER_PLUGIN_PRE_UPGRADE"},   // NOLINT
    {{ActionType::Uninstall, ProcessType::PreInstall},  "PKGMGR_PARSER_PLUGIN_PRE_UNINSTALL"}, // NOLINT
    {{ActionType::Install,   ProcessType::Install},     "PKGMGR_PARSER_PLUGIN_INSTALL"},       // NOLINT
    {{ActionType::Upgrade,   ProcessType::Install},     "PKGMGR_PARSER_PLUGIN_UPGRADE"},       // NOLINT
    {{ActionType::Uninstall, ProcessType::Install},     "PKGMGR_PARSER_PLUGIN_UNINSTALL"},     // NOLINT
    {{ActionType::Install,   ProcessType::PostInstall}, "PKGMGR_PARSER_PLUGIN_POST_INSTALL"},  // NOLINT
    {{ActionType::Upgrade,   ProcessType::PostInstall}, "PKGMGR_PARSER_PLUGIN_POST_UPGRADE"},  // NOLINT
    {{ActionType::Uninstall, ProcessType::PostInstall}, "PKGMGR_PARSER_PLUGIN_POST_UNINSTALL"} // NOLINT
  };

  auto pos = names.find(std::make_pair(action, process));
  if (pos == names.end()) {
    LOG(ERROR) << "Function name not defined";
    return false;
  }
  *result = pos->second;
  return true;
}

bool PluginsLauncher::ExecPlugin(DynamicLibHandle& dlh, ProcessType process,
                                 ActionType action, const char* pkgId,
                                 int* result) {
  return ExecPluginImpl(dlh, process, action, result, pkgId);
}

bool PluginsLauncher::ExecPlugin(DynamicLibHandle& dlh, ProcessType process,
                                 ActionType action, xmlDocPtr docPtr,
                                 const char* pkgId, int* result) {
  return ExecPluginImpl(dlh, process, action, result, docPtr, pkgId);
}

}  // namespace common_installer
