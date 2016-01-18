// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include <string>
#include <vector>

#include "common/utils/plugin_list_parser.h"
#include "common/utils/plugin_xml_parser.h"
#include "common/utils/plugin_manager.h"

namespace common_installer {

bool PluginManager::GenerateUnknownTagList() {
  tags_.clear();

  if (!xmlParser_.Parse()) {
    LOG(ERROR) << "Parse xml function error";
    return false;
  }

  std::vector<std::string> xmlTags = xmlParser_.Data();

  if (!listParser_.Parse()) {
    LOG(ERROR) << "Parse list function error";
    return false;
  }

  const std::vector<std::shared_ptr<PluginInfo>> listTags = listParser_.Data();

  for (std::shared_ptr<PluginInfo> pluginInfo : listTags) {
    // find only tags
    if (pluginInfo->Type() == "tag") {
      for (const std::string xmlTag : xmlTags) {
        // if system tags included in xml tags
        if (pluginInfo->Name() == xmlTag) {
          tags_.push_back(pluginInfo);
        }
      }
    }
  }

  return true;
}

std::vector<std::shared_ptr<PluginInfo>>& PluginManager::UnknownTagList() {
  return tags_;
}

bool PluginManager::Launch(const boost::filesystem::path& pluginPath,
                           ActionType actionType, const std::string& pkgId) {
  // TODO(l.wartalowic) add implementation
  (void)pluginPath;
  (void)actionType;
  (void)pkgId;
  return true;
}

}  // namespace common_installer
