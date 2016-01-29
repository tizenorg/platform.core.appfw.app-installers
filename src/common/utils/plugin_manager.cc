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

  const PluginsListParser::PluginList& pluginInfoList =
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

const PluginManager::TagList& PluginManager::UnknownTagList() { return tags_; }

xmlDocPtr PluginManager::CreateDocPtrForPlugin(xmlDocPtr doc_ptr,
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

bool PluginManager::Launch(const boost::filesystem::path& plugin_path,
                           const std::string& tag_name,
                           PluginsLauncher::ActionType action_type,
                           const std::string& pkg_Id) {
  LOG(INFO) << "Launching plugin path:" << plugin_path << " pkgId: " << pkg_Id;

  int result = EPERM;

  xmlDocPtr plugin_doc_ptr = CreateDocPtrForPlugin(xml_parser_.doc_ptr(),
                                                   tag_name);
  if (!plugin_doc_ptr)
    return false;
  PluginsLauncher::Error error = plugins_launcher_.LaunchPlugin(
      plugin_path, plugin_doc_ptr, action_type, pkg_Id, &result);
  xmlFreeDoc(plugin_doc_ptr);

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
