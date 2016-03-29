// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_PLUGINS_PLUGIN_FACTORY_H_
#define COMMON_PLUGINS_PLUGIN_FACTORY_H_

#include <memory>

#include "common/plugins/plugin.h"
#include "common/plugins/plugin_list_parser.h"

namespace common_installer {

__attribute__ ((visibility ("default"))) class PluginFactory {
 public:
  std::unique_ptr<Plugin> CreatePluginByPluginInfo(
      const PluginInfo& plugin_info);
};

}  // namespace common_installer

#endif  // COMMON_PLUGINS_PLUGIN_FACTORY_H_
