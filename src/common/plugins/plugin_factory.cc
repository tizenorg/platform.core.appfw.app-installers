// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/plugins/plugin_factory.h"

#include <manifest_parser/utils/logging.h>

#include "common/plugins/types/category_plugin.h"
#include "common/plugins/types/metadata_plugin.h"
#include "common/plugins/types/tag_plugin.h"

namespace common_installer {

std::unique_ptr<Plugin> PluginFactory::CreatePluginByPluginInfo(
    const PluginInfo& plugin_info) {
  if (plugin_info.type() == TagPlugin::kType) {
    return TagPlugin::Create(plugin_info);
  } else if (plugin_info.type() == MetadataPlugin::kType) {
    return MetadataPlugin::Create(plugin_info);
  } else if (plugin_info.type() == CategoryPlugin::kType) {
    return CategoryPlugin::Create(plugin_info);
  } else {
    LOG(ERROR) << "Unknown plugin type: " << plugin_info.type();
    return nullptr;
  }
}

}  // namespace common_installer
