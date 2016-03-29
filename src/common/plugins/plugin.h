// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_PLUGINS_PLUGIN_H_
#define COMMON_PLUGINS_PLUGIN_H_

#include <boost/filesystem/path.hpp>
#include <dlfcn.h>
#include <manifest_parser/utils/logging.h>
#include <libxml2/libxml/tree.h>
#include <pkgmgrinfo_basic.h>

#include <memory>
#include <string>

#include "common/plugins/plugin_list_parser.h"

namespace common_installer {

/**
 * @brief The Plugin class
 *        Represents pkgmgr plugin object handling internally managing loading
 *        of .so shared object from file system. Run() function is being
 *        overloaded in subclasses to accommodate plugin differences.
 */
class __attribute__ ((visibility ("default"))) Plugin {
 public:
  enum class ActionType { Install, Upgrade, Uninstall };
  enum class ProcessType { Pre, Main, Post };

  virtual ~Plugin();

  template <typename Ret, typename... Args>
  bool Exec(const std::string& name, Ret* result, Args... args) {
    using PluginFunctionPtr = Ret (*)(Args...);
    PluginFunctionPtr function =
        reinterpret_cast<PluginFunctionPtr>(GetSymbol(name));

    if (!function) {
      LOG(WARNING) << "Skip to execute symbol: " << name << " (" << dlerror()
                   << ")";
      return false;
    }

    LOG(DEBUG) << "Execute plugin function: " << name << " of "
               << plugin_info_.path() << "...";
    *result = function(args...);
    return true;
  }

  virtual bool Run(xmlDocPtr doc_ptr, manifest_x* manifest,
                   ActionType action_type) = 0;

  Plugin(Plugin&&) = default;
  Plugin& operator=(Plugin&&) = default;

 protected:
  explicit Plugin(const PluginInfo& plugin_info);
  bool Load();

  PluginInfo plugin_info_;

 private:
  void* GetSymbol(const std::string& name) const;

  void* lib_handle_;

  SCOPE_LOG_TAG(Plugin)
};

/**
 * @brief ActionTypeToPkgmgrActionType
 *        Helper function to convert app-installer ActionType to pkgmgr action
 *        type for 'category' and 'metadata' plugins.
 *
 * @param action input action type
 * @return pkgmgr action type or -1 if error
 */
__attribute__ ((visibility ("default"))) int ActionTypeToPkgmgrActionType(common_installer::Plugin::ActionType action);

}  // namespace common_installer

#endif  // COMMON_PLUGINS_PLUGIN_H_
