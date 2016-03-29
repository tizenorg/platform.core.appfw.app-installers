// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_PLUGINS_PLUGIN_MANAGER_H_
#define COMMON_PLUGINS_PLUGIN_MANAGER_H_

#include <boost/filesystem/path.hpp>

#include <pkgmgrinfo_basic.h>
#include <string>
#include <vector>

#include "common/plugins/plugin.h"
#include "common/plugins/plugin_list_parser.h"
#include "common/plugins/plugin_xml_parser.h"

namespace common_installer {

/** this class manages XML and plugin lists */
class PluginManager {
 public:
  using PluginInfoList = std::vector<std::shared_ptr<PluginInfo>>;

  __attribute__ ((visibility ("default"))) PluginManager(const std::string& xml_path,
                const std::string& list_path,
                manifest_x* manifest)
      : xml_parser_(xml_path),
        list_parser_(list_path),
        manifest_(manifest) {}

  __attribute__ ((visibility ("default"))) bool LoadPlugins();
  __attribute__ ((visibility ("default"))) void RunPlugins(Plugin::ActionType action_type);

 private:
  __attribute__ ((visibility ("default"))) bool GenerateUnknownTagList(std::vector<std::string>* xml_tags);
  __attribute__ ((visibility ("default"))) bool GeneratePluginInfoList(PluginInfoList* plugin_info_list);

  PluginsXmlParser xml_parser_;
  PluginsListParser list_parser_;
  manifest_x* manifest_;
  std::vector<std::unique_ptr<Plugin>> loaded_plugins_;
};

}  // namespace common_installer

#endif  // COMMON_PLUGINS_PLUGIN_MANAGER_H_
