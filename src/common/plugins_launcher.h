// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_PLUGINS_LAUNCHER_H_
#define COMMON_PLUGINS_LAUNCHER_H_

#include <boost/filesystem/path.hpp>

#include <libxml/tree.h>
#include <manifest_parser/utils/logging.h>

#include <string>
#include <memory>

#include "common/utils/dynamic_lib_handle.h"

namespace common_installer {


class PluginsLauncher {
 public:
  enum class ActionType { Install, Upgrade, Uninstall };
  enum class ProcessType  {
    NoProcess = 0x00,
    PreInstall = 0x01,
    Install = 0x02,
    PostInstall = 0x04
  };

  enum class Error { Success, FailedLibHandle, ActionNotSupported };

  Error LaunchPlugin(const boost::filesystem::path& plugin_path,
                    xmlDocPtr docPtr, ActionType action,
                    const std::string& pkgId, int* result);

 private:
  bool FunctionName(ProcessType process, ActionType action,
                    std::string* result);

  bool ExecPlugin(DynamicLibHandle& dlh, ProcessType process, ActionType action,
                  const char* pkgId, int* result);

  bool ExecPlugin(DynamicLibHandle& dlh, ProcessType process, ActionType action,
                  xmlDocPtr docPtr, const char* pkgId, int* result);

  template <typename... Args>
  bool ExecPluginImpl(DynamicLibHandle& dlh, ProcessType process,
                      ActionType action, int* result, Args&&... args) {
    std::string name;
    if (!FunctionName(process, action, &name)) {
      LOG(ERROR) << "Error during getting function name";
      return false;
    }
    return dlh.Exec(name, result, std::forward<Args>(args)...);
  }

  SCOPE_LOG_TAG(PluginsLauncher)
};

inline PluginsLauncher::ProcessType operator|(
    PluginsLauncher::ProcessType lhs, PluginsLauncher::ProcessType rhs) {
  return static_cast<PluginsLauncher::ProcessType>(static_cast<int>(lhs) |
                                                   static_cast<int>(rhs));
}

inline PluginsLauncher::ProcessType& operator|=(
    PluginsLauncher::ProcessType& lhs, PluginsLauncher::ProcessType rhs) {
  return lhs = lhs | rhs;
}

}  // namespace common_installer

#endif  // COMMON_PLUGINS_LAUNCHER_H_
