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
  enum class ProcessType { PreInstall, Install, PostInstall };

  bool LaunchPlugin(const boost::filesystem::path& plugin_path,
                    xmlDocPtr docPtr, ActionType action,
                    const std::string& pkgId, int* result);

 private:
  bool FunctionName(ProcessType process, ActionType action,
                    std::string* result);

  bool ExecPlugin(DynamicLibHandle* dlh, ProcessType process,
                  ActionType action, const char* pkgId, int* result);

  bool ExecPlugin(DynamicLibHandle* dlh, ProcessType process,
                  ActionType action, xmlDocPtr docPtr, const char* pkgId,
                  int* result);

  template <typename... Args>
  bool ExecPluginImpl(DynamicLibHandle* dlh, ProcessType process,
                             ActionType action, int *result, Args&&... args) {
    std::string name;
    if (!FunctionName(process, action, &name)) {
      LOG(ERROR) << "Error during getting function name";
      return false;
    }
    return dlh->Exec(name, result, std::forward<Args>(args)...);
  }

  SCOPE_LOG_TAG(PluginsLauncher)
};

}  // namespace common_installer

#endif  // COMMON_PLUGINS_LAUNCHER_H_
