// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_UTILS_PLUGIN_MANAGER_H_
#define COMMON_UTILS_PLUGIN_MANAGER_H_

#include <string>
#include <vector>

#include "common/utils/plugin_xml_parser.h"
#include "common/utils/plugin_list_parser.h"

namespace common_installer {

/** this class manages XML and plugin lists */
class PluginManager {
 public:
  PluginManager(const std::string& xmlPath, const std::string& listPath)
      : xmlParser_(xmlPath), listParser_(listPath) {}

  bool GenerateUnknownTagList();
  std::vector<std::shared_ptr<PluginInfo>>& UnknownTagList();
  xmlDocPtr DocPtr();

 private:
  std::vector<std::shared_ptr<PluginInfo>> tags_;
  PluginsXmlParser xmlParser_;
  PluginsListParser listParser_;
};
}  // namespace common_installer

#endif  // COMMON_UTILS_PLUGIN_MANAGER_H_
