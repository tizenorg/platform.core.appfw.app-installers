// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_PLUGINS_TYPES_TAG_PLUGIN_H_
#define COMMON_PLUGINS_TYPES_TAG_PLUGIN_H_

#include <manifest_parser/utils/logging.h>

#include <memory>
#include <string>

#include "common/plugins/plugin.h"
#include "common/plugins/plugin_list_parser.h"

namespace common_installer {

class __attribute__ ((visibility ("default"))) TagPlugin : public Plugin {
 public:
  static const char kType[];

  static std::unique_ptr<TagPlugin> Create(const PluginInfo& plugin_info);

  bool Run(xmlDocPtr doc_ptr, manifest_x* manifest,
           ActionType action_type) override;

 private:
  using Plugin::Plugin;
  std::string GetFunctionName(ProcessType process, ActionType action) const;

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

  SCOPE_LOG_TAG(TagPlugin)
};

}  // namespace common_installer

#endif  // COMMON_PLUGINS_TYPES_TAG_PLUGIN_H_
