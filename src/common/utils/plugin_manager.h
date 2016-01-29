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
#include "common/plugins_launcher.h"

namespace common_installer {

/** this class manages XML and plugin lists */
class PluginManager {
 public:
  using TagList = std::vector<std::shared_ptr<PluginInfo>>;
  PluginManager(const std::string& xml_path, const std::string& list_path)
      : xml_parser_(xml_path), list_parser_(list_path) {}

  bool GenerateUnknownTagList();
  const TagList& UnknownTagList();
  bool Launch(const boost::filesystem::path& pluginPath,
              const std::string& tag_name,
              PluginsLauncher::ActionType actionType, const std::string& pkgId);

 private:
  /**
   * @brief CreateDocPtrForPlugin
   *        Create copy of xml document with nodes only matching requested
   *        tag_name
   * @param doc_ptr original doc ptr of document
   * @param tag_name name of required node/nodes
   * @return requested copy
   */
  xmlDocPtr CreateDocPtrForPlugin(xmlDocPtr doc_ptr,
                                  const std::string& tag_name) const;

  std::vector<std::shared_ptr<PluginInfo>> tags_;
  PluginsXmlParser xml_parser_;
  PluginsListParser list_parser_;
  PluginsLauncher plugins_launcher_;
};
}  // namespace common_installer

#endif  // COMMON_UTILS_PLUGIN_MANAGER_H_
