// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/plugins/types/metadata_plugin.h"

#include <pkgmgr_parser.h>

#include <map>

#include "common/utils/glist_range.h"

namespace {

std::string GetMetadataTag(const std::string& url) {
  return url.substr(url.find_last_of('/') + 1);
}

void ClearMetadataDetail(gpointer data) {
  __metadata_t* meta = reinterpret_cast<__metadata_t*>(data);
  free(const_cast<char*>(meta->key));
  free(const_cast<char*>(meta->value));
  free(meta);
}

}  // namespace

namespace common_installer {

const char MetadataPlugin::kType[] = "metadata";

std::unique_ptr<MetadataPlugin> MetadataPlugin::Create(
    const PluginInfo& plugin_info) {
  std::unique_ptr<MetadataPlugin> plugin(new MetadataPlugin(plugin_info));
  if (!plugin->Load())
    return nullptr;
  return plugin;
}

std::string MetadataPlugin::GetFunctionName(ActionType action) const {
  static std::map<ActionType, std::string> names {
    {ActionType::Install,  "PKGMGR_MDPARSER_PLUGIN_INSTALL"},
    {ActionType::Upgrade,  "PKGMGR_MDPARSER_PLUGIN_UPGRADE"},
    {ActionType::Uninstall,  "PKGMGR_MDPARSER_PLUGIN_UNINSTALL"},
  };

  auto pos = names.find(action);
  if (pos == names.end()) {
    LOG(ERROR) << "Function name not defined";
    return "";
  }
  return pos->second;
}

bool MetadataPlugin::Run(xmlDocPtr /*doc_ptr*/, manifest_x* manifest,
                         ActionType action_type) {
  std::string tag = GetMetadataTag(plugin_info_.name());
  if (tag.empty())
    return false;
  std::string name = GetFunctionName(action_type);
  for (application_x* app : GListRange<application_x*>(manifest->application)) {
    // pack all metadata starting with key + '/' to list that will
    // be sent to the plugin.
    // e.g. all http://developer.samsung.com/tizen/metadata/profile/*
    //   will be packed for http://developer.samsung.com/tizen/metadata/profile
    GList* md_list = nullptr;
    for (metadata_x* meta : GListRange<metadata_x*>(app->metadata)) {
      std::string sub_key_prefix = plugin_info_.name() + "/";
      if (meta->key && meta->value &&
          std::string(meta->key).find(sub_key_prefix) == 0) {
        __metadata_t* md = reinterpret_cast<__metadata_t*>(
            calloc(1, sizeof(__metadata_t)));
        md->key = strdup(meta->key);
        md->value = strdup(meta->value);
        md_list = g_list_append(md_list, md);
      }
    }
    int result = 0;
    Exec(name, &result, md_list, tag.c_str(),
         ActionTypeToPkgmgrActionType(action_type),
         manifest->package, app->appid);
    if (result) {
      LOG(ERROR) << "Function: " << name << " of plugin "
                 << plugin_info_.path() << " failed";
      g_list_free_full(md_list, &ClearMetadataDetail);
      return false;
    }
    g_list_free_full(md_list, &ClearMetadataDetail);
  }
  return true;
}

}  // namespace common_installer
