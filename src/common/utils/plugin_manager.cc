// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include <boost/filesystem.hpp>

#include <string>
#include <vector>

#include "common/utils/plugin_list_parser.h"
#include "common/utils/plugin_xml_parser.h"
#include "common/utils/plugin_manager.h"

namespace common_installer {

bool PluginManager::GenerateUnknownTagList() {
  tags_.clear();

  if (!xml_parser_.Parse()) {
    LOG(ERROR) << "Parse xml function error";
    return false;
  }

  std::vector<std::string> xmlTags = xml_parser_.tags_list();

  if (!list_parser_.Parse()) {
    LOG(ERROR) << "Parse list function error";
    return false;
  }

  const std::vector<std::shared_ptr<PluginInfo>> pluginInfoList =
      list_parser_.PluginInfoList();

  for (std::shared_ptr<PluginInfo> pluginInfo : pluginInfoList) {
    // find only tags
    if (pluginInfo->type() == "tag") {
      // check if a file exist
      if (boost::filesystem::exists(pluginInfo->path())) {
        for (const std::string& xmlTag : xmlTags) {
          // if system tags included in xml tags
          if (pluginInfo->name() == xmlTag) {
            tags_.push_back(pluginInfo);
            LOG(DEBUG) << "Tag: " << pluginInfo->name()
                       << " path: " << pluginInfo->path() << "has been added";
            break;
          }
        }
      } else {
        LOG(WARNING) << "Tag: " << pluginInfo->name()
                     << " path: " << pluginInfo->path()
                     << " exist in plugin list but no exist in system.";
      }
    }
  }

  if (tags_.empty()) {
    LOG(INFO) << "No tags to processing";
  }

  return true;
}

const std::vector<std::shared_ptr<PluginInfo>>&
PluginManager::UnknownTagList() {
  return tags_;
}

bool PluginManager::Launch(const boost::filesystem::path& plugin_path,
                           PluginsLauncher::ActionType action_type,
                           const std::string& pkg_Id) {
  LOG(INFO) << "Launching plugin path:" << plugin_path << " pkgId: " << pkg_Id;

  int result = EPERM;

  PluginsLauncher::Error error = plugins_launcher_.LaunchPlugin(
      plugin_path, xml_parser_.doc_ptr(), action_type, pkg_Id, &result);

  switch (error) {
    case PluginsLauncher::Error::Success: {
      if (result != 0) {
        LOG(ERROR) << "Error from plugin lib: " << plugin_path
                   << " error code: " << result;
        return false;
      }
      return true;
    }
    case PluginsLauncher::Error::ActionNotSupported:
      return true;

    case PluginsLauncher::Error::FailedLibHandle:
    default:
      return false;
  }
}

}  // namespace common_installer
