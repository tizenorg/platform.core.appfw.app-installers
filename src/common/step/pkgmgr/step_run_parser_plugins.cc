// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/pkgmgr/step_run_parser_plugins.h"

#include <string>
#include <vector>

#include "common/plugins/plugin_manager.h"

namespace common_installer {
namespace pkgmgr {

StepRunParserPlugin::StepRunParserPlugin(
    InstallerContext* context, Plugin::ActionType action_type)
    : Step(context), action_type_(action_type) {}

Step::Status StepRunParserPlugin::ProcessPlugins(
    const boost::filesystem::path& xml_path, manifest_x* manifest,
    Plugin::ActionType action_type) {
  // PLUGINS_LIST_FILE_PATH path generated from cmake
  const std::string listPath(PLUGINS_LIST_INSTALL_FILE_PATH);

  PluginManager plugin_manager(xml_path.string(), listPath, manifest);
  if (!plugin_manager.LoadPlugins()) {
    LOG(ERROR) << "Loading plugins failed in progress";
    return Status::ERROR;
  }
  plugin_manager.RunPlugins(action_type);

  LOG(INFO) << "Processing plugins done";
  return Status::OK;
}

Step::Status StepRunParserPlugin::process() {
  return ProcessPlugins(context_->xml_path.get(), context_->manifest_data.get(),
                        action_type_);
}

Step::Status StepRunParserPlugin::undo() {
  // For update, we need to reread configuration of old version of widget
  // so running whole process from beginning
  if (action_type_ == Plugin::ActionType::Install) {
    ProcessPlugins(context_->xml_path.get(), context_->manifest_data.get(),
                   Plugin::ActionType::Uninstall);
  } else if (action_type_ == Plugin::ActionType::Upgrade) {
    ProcessPlugins(context_->backup_xml_path.get(),
                   context_->old_manifest_data.get(),
                   Plugin::ActionType::Upgrade);
  }
  return Status::OK;
}

}  // namespace pkgmgr
}  // namespace common_installer
