// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_UTILS_PLUGIN_MANAGER_H_
#define COMMON_UTILS_PLUGIN_MANAGER_H_

#include <boost/filesystem/path.hpp>

#include <string>
#include <vector>

#include "common/utils/plugin_xml_parser.h"
#include "common/utils/plugin_list_parser.h"

namespace common_installer {

enum class ActionType { Install, Upgrade, Uninstall };

/** this class manages XML and plugin lists */
class PluginManager {
 public:
  PluginManager(const std::string& xml_path, const std::string& list_path)
      : xml_parser_(xml_path), list_parser_(list_path) {}

  bool GenerateUnknownTagList();
  std::vector<std::shared_ptr<PluginInfo>>& UnknownTagList();
  bool Launch(const boost::filesystem::path& pluginPath, ActionType actionType,
              const std::string& pkgId);

 private:
  std::vector<std::shared_ptr<PluginInfo>> tags_;
  PluginsXmlParser xml_parser_;
  PluginsListParser list_parser_;
};
}  // namespace common_installer

#endif  // COMMON_UTILS_PLUGIN_MANAGER_H_
