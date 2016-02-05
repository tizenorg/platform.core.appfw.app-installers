// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/plugins/types/category_plugin.h"

#include <pkgmgr_parser.h>

#include <map>

#include "common/utils/glist_range.h"

namespace {

std::string GetCategoryName(const std::string& url) {
  return url.substr(url.find_last_of('/') + 1);
}

void ClearCategoryDetail(gpointer data) {
  __category_t* category = reinterpret_cast<__category_t*>(data);
  free(const_cast<char*>(category->name));
  free(category);
}

}  // namespace

namespace common_installer {

const char CategoryPlugin::kType[] = "category";

std::unique_ptr<CategoryPlugin> CategoryPlugin::Create(
    const PluginInfo& plugin_info) {
  std::unique_ptr<CategoryPlugin> plugin(new CategoryPlugin(plugin_info));
  if (!plugin->Load())
    return nullptr;
  return plugin;
}

std::string CategoryPlugin::GetFunctionName(ActionType action) const {
  static std::map<ActionType, std::string> names {
    {ActionType::Install,  "PKGMGR_CATEGORY_PARSER_PLUGIN_INSTALL"},
    {ActionType::Upgrade,  "PKGMGR_CATEGORY_PARSER_PLUGIN_UPGRADE"},
    {ActionType::Uninstall,  "PKGMGR_CATEGORY_PARSER_PLUGIN_UNINSTALL"},
  };

  auto pos = names.find(action);
  if (pos == names.end()) {
    LOG(ERROR) << "Function name not defined";
    return "";
  }
  return pos->second;
}

bool CategoryPlugin::Run(xmlDocPtr /*doc_ptr*/, manifest_x* manifest,
         ActionType action_type) {
  std::string tag = GetCategoryName(plugin_info_.name());
  if (tag.empty())
    return false;
  std::string name = GetFunctionName(action_type);
  for (application_x* app : GListRange<application_x*>(manifest->application)) {
    // pack all categories starting with key + '/' to list that will
    // be sent to the plugin.
    // e.g. all http://tizen.org/category/antivirus/*
    //   will be packed for http://tizen.org/category/antivirus
    GList* category_list = nullptr;
    for (const char* category : GListRange<char*>(app->category)) {
      std::string sub_key_prefix = plugin_info_.name() + "/";
      if (std::string(category).find(sub_key_prefix) == 0) {
        __category_t* c = reinterpret_cast<__category_t*>(
            calloc(1, sizeof(__category_t)));
        c->name = strdup(category);
        category_list = g_list_append(category_list, c);
      }
    }
    int result = 0;
    Exec(name, &result, category_list, tag.c_str(),
         ActionTypeToPkgmgrActionType(action_type),
         manifest->package, app->appid);
    if (result) {
      LOG(ERROR) << "Function: " << name << " of plugin "
                 << plugin_info_.path() << " failed";
      g_list_free_full(category_list, &ClearCategoryDetail);
      return false;
    }
    g_list_free_full(category_list, &ClearCategoryDetail);
  }
}

}  // namespace common_installer
