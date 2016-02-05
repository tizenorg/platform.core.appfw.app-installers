// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/plugins/plugin_manager.h"

#include <boost/filesystem.hpp>

#include <algorithm>
#include <string>
#include <vector>

#include "common/plugins/plugin_factory.h"
#include "common/plugins/plugin_list_parser.h"
#include "common/plugins/plugin_xml_parser.h"
#include "common/plugins/types/category_plugin.h"
#include "common/plugins/types/metadata_plugin.h"
#include "common/plugins/types/tag_plugin.h"
#include "common/utils/glist_range.h"

namespace common_installer {

bool PluginManager::GenerateUnknownTagList(
    std::vector<std::string>* xml_tags) {
  if (!xml_parser_.Parse()) {
    LOG(ERROR) << "Parse xml function error";
    return false;
  }

  *xml_tags = xml_parser_.tags_list();
  return true;
}

bool PluginManager::GeneratePluginInfoList(
    PluginManager::PluginInfoList* plugin_info_list) {
  if (!list_parser_.Parse()) {
    LOG(ERROR) << "Parse list function error";
    return false;
  }

  *plugin_info_list = list_parser_.PluginInfoList();
  return true;
}

bool PluginManager::LoadPlugins() {
  std::vector<std::string> xml_tags;
  if (!GenerateUnknownTagList(&xml_tags))
    return false;

  PluginInfoList plugin_info_list;
  if (!GeneratePluginInfoList(&plugin_info_list))
    return false;

  PluginFactory factory;

  std::sort(xml_tags.begin(), xml_tags.end());

  // This loop loads plugin which are needed according to manifest file
  // Different pkgmgr plugin types have different condition upon which they
  // are being loaded
  LOG(DEBUG) << "Loading pkgmgr plugins...";
  for (std::shared_ptr<PluginInfo> plugin_info : plugin_info_list) {
    std::unique_ptr<Plugin> plugin;
    if (plugin_info->type() == TagPlugin::kType) {
      // load tag plugin only if tag exists in manifest file
      auto iter = std::lower_bound(xml_tags.begin(), xml_tags.end(),
                                     plugin_info->name());
      if (iter != xml_tags.end() && *iter == plugin_info->name()) {
        plugin = factory.CreatePluginByPluginInfo(*plugin_info);
        if (!plugin) {
          LOG(ERROR) << "Failed to load plugin: " << plugin_info->path()
                     << " Plugin has been skipped.";
        }
      }
    } else if (plugin_info->type() == MetadataPlugin::kType) {
      bool done = false;
      for (application_x* app : GListRange<application_x*>(
           manifest_->application)) {
        for (metadata_x* meta : GListRange<metadata_x*>(app->metadata)) {
          if (std::string(meta->key).find(plugin_info->name()) == 0) {
            plugin = factory.CreatePluginByPluginInfo(*plugin_info);
            if (!plugin) {
              LOG(ERROR) << "Failed to load plugin: " << plugin_info->path()
                         << " Plugin has been skipped.";
            }
            done = true;
            break;
          }
        }
        if (done)
          break;
      }
    } else if (plugin_info->type() == CategoryPlugin::kType) {
      bool done = false;
      for (application_x* app : GListRange<application_x*>(
           manifest_->application)) {
        for (const char* category : GListRange<char*>(app->category)) {
          if (std::string(category).find(plugin_info->name()) == 0) {
            plugin = factory.CreatePluginByPluginInfo(*plugin_info);
            if (!plugin) {
              LOG(ERROR) << "Failed to load plugin: " << plugin_info->path()
                         << " Plugin has been skipped.";
            }
            done = true;
            break;
          }
        }
        if (done)
          break;
      }
    }

    if (plugin) {
      loaded_plugins_.push_back(std::move(plugin));
      LOG(DEBUG) << "Loaded plugin: " << plugin_info->path();
    }
  }
  return true;
}

void PluginManager::RunPlugins(Plugin::ActionType action_type) {
  LOG(DEBUG) << "Running pkgmgr plugins...";
  for (auto& plugin : loaded_plugins_) {
    // FIXME: Ignore if plugin failed for now, we need to keep installation
    // working nevertheless plugins are broken
    plugin->Run(xml_parser_.doc_ptr(), manifest_, action_type);
  }
}

}  // namespace common_installer
