// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_PLUGINS_LAUNCHER_H_
#define COMMON_PLUGINS_LAUNCHER_H_

#include <libxml/tree.h>
#include <manifest_parser/utils/logging.h>

#include <string>
#include <vector>
#include <memory>

#include "utils/dynamic_lib_handle.h"

namespace common_installer {

enum class ActionType { Install, Upgrade, Uninstall };
enum class ProcessType { PreInstall, Install, PostInstall };

class PluginsLauncher {
 public:
  bool LaunchPlugins(const std::vector<std::string>& plugin_names,
                     xmlDocPtr docPtr, ActionType action,
                     const std::string& pkgId, int *result);

 private:
  std::string GetPath(const std::string& plugin);
  bool GetName(ProcessType process, ActionType action, std::string *result);

  bool RunPluginFunction(DynamicLibHandle *dl_handle, ProcessType process,
                         ActionType action, const char *pkgId, int *result);

  bool RunPluginFunction(DynamicLibHandle *dl_handle, ProcessType process,
                         ActionType action, xmlDocPtr docPtr, const char *pkgId,
                         int *result);

  template <typename... Args>
  bool RunPluginFunctionImpl(DynamicLibHandle *dl_handle, ProcessType process,
                             ActionType action, int *result, Args... args) {
    std::unique_ptr<std::string> name(new std::string);
    if (!GetName(process, action, name.get())) {
      LOG(ERROR) << "Error during getting function name";
      return false;
    }
    return dl_handle->run(*name, result, args...);
  }

  SCOPE_LOG_TAG(PluginsLauncher)
};

}  // namespace common_installer

#endif  // COMMON_PLUGINS_LAUNCHER_H_
