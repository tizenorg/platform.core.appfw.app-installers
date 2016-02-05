// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_PLUGINS_TYPES_CATEGORY_PLUGIN_H_
#define COMMON_PLUGINS_TYPES_CATEGORY_PLUGIN_H_

#include <string>

#include "common/plugins/plugin.h"

namespace common_installer {

class CategoryPlugin : public Plugin {
 public:
  static const char kType[];

  static std::unique_ptr<CategoryPlugin> Create(const PluginInfo& plugin_info);
  bool Run(xmlDocPtr doc_ptr, manifest_x* manifest,
           ActionType action_type) override;

 private:
  std::string GetFunctionName(ActionType action) const;

  using Plugin::Plugin;

  SCOPE_LOG_TAG(CategoryPlugin)
};

}  // namespace common_installer

#endif  // COMMON_PLUGINS_TYPES_CATEGORY_PLUGIN_H_
