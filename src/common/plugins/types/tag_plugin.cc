// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/plugins/types/tag_plugin.h"

#include <map>
#include <utility>

namespace common_installer {

const char TagPlugin::kType[] = "tag";

std::unique_ptr<TagPlugin> TagPlugin::Create(const PluginInfo& plugin_info) {
  std::unique_ptr<TagPlugin> plugin(new TagPlugin(plugin_info));
  if (!plugin->Load())
    return nullptr;
  return plugin;
}

std::string TagPlugin::GetFunctionName(ProcessType process,
                                       ActionType action) const {
  static std::map<std::pair<ActionType, ProcessType>, std::string> names {
    {{ActionType::Install,   ProcessType::Pre},  "PKGMGR_PARSER_PLUGIN_PRE_INSTALL"},   // NOLINT
    {{ActionType::Upgrade,   ProcessType::Pre},  "PKGMGR_PARSER_PLUGIN_PRE_UPGRADE"},   // NOLINT
    {{ActionType::Uninstall, ProcessType::Pre},  "PKGMGR_PARSER_PLUGIN_PRE_UNINSTALL"}, // NOLINT
    {{ActionType::Install,   ProcessType::Main}, "PKGMGR_PARSER_PLUGIN_INSTALL"},       // NOLINT
    {{ActionType::Upgrade,   ProcessType::Main}, "PKGMGR_PARSER_PLUGIN_UPGRADE"},       // NOLINT
    {{ActionType::Uninstall, ProcessType::Main}, "PKGMGR_PARSER_PLUGIN_UNINSTALL"},     // NOLINT
    {{ActionType::Install,   ProcessType::Post}, "PKGMGR_PARSER_PLUGIN_POST_INSTALL"},  // NOLINT
    {{ActionType::Upgrade,   ProcessType::Post}, "PKGMGR_PARSER_PLUGIN_POST_UPGRADE"},  // NOLINT
    {{ActionType::Uninstall, ProcessType::Post}, "PKGMGR_PARSER_PLUGIN_POST_UNINSTALL"} // NOLINT
  };

  auto pos = names.find(std::make_pair(action, process));
  if (pos == names.end()) {
    LOG(ERROR) << "Function name not defined";
    return "";
  }
  return pos->second;
}

xmlDocPtr TagPlugin::CreateDocPtrForPlugin(xmlDocPtr doc_ptr,
    const std::string& tag_name) const {
  // Make copy of document and root node
  xmlNodePtr root_node = xmlDocGetRootElement(doc_ptr);
  if (!root_node) {
    LOG(ERROR) << "Original document is empty. Cannot create copy for plugin";
    return nullptr;
  }
  xmlDocPtr plugin_doc_ptr = xmlCopyDoc(doc_ptr, 0);
  xmlNodePtr plugin_root_node = xmlCopyNode(root_node, 0);
  xmlDocSetRootElement(plugin_doc_ptr, plugin_root_node);

  // Append elements that matches the tag name to new doc
  for (xmlNodePtr child = xmlFirstElementChild(root_node);
       child != nullptr; child = xmlNextElementSibling(child)) {
    if (tag_name == reinterpret_cast<const char*>(child->name)) {
      xmlAddChild(plugin_root_node, xmlCopyNode(child, 1));
    }
  }
  xmlSetTreeDoc(plugin_root_node, plugin_doc_ptr);
  return plugin_doc_ptr;
}

bool TagPlugin::Run(xmlDocPtr doc_ptr, manifest_x* manifest,
         ActionType action_type) {
  xmlDocPtr plugin_doc_ptr = CreateDocPtrForPlugin(doc_ptr,
                                                   plugin_info_.name());
  if (!plugin_doc_ptr)
    return false;

  int result = 0;
  std::string pre_function = GetFunctionName(ProcessType::Pre, action_type);
  Exec(pre_function, &result, manifest->package);
  if (result) {
    LOG(ERROR) << "Function: " << pre_function << " of plugin "
               << plugin_info_.path() << " failed";
    xmlFreeDoc(plugin_doc_ptr);
    return false;
  }

  std::string main_function = GetFunctionName(ProcessType::Main, action_type);
  Exec(main_function, &result, plugin_doc_ptr, manifest->package);
  if (result) {
    LOG(ERROR) << "Function: " << main_function << " of plugin "
               << plugin_info_.path() << " failed";
    xmlFreeDoc(plugin_doc_ptr);
    return false;
  }

  std::string post_function = GetFunctionName(ProcessType::Post, action_type);
  Exec(post_function, &result, manifest->package);
  if (result) {
    LOG(ERROR) << "Function: " << post_function << " of plugin "
               << plugin_info_.path() << " failed";
    xmlFreeDoc(plugin_doc_ptr);
    return false;
  }

  xmlFreeDoc(plugin_doc_ptr);
  return true;
}


}  // namespace common_installer
