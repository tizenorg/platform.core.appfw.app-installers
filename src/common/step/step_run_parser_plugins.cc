// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include <string>
#include <vector>

#include "common/step/step_run_parser_plugins.h"

namespace common_installer {
namespace pkgmgr {

StepRunParserPlugin::StepRunParserPlugin(InstallerContext* context,
                                         ActionType action_type)
    : Step(context), action_type_(action_type) {}

Step::Status StepRunParserPlugin::ProcessingPlugins(
    const boost::filesystem::path& xml_path) {
  // PLUGINS_LIST_FILE_PATH path generated from cmake
  const std::string listPath(PLUGINS_LIST_INSTALL_FILE_PATH);

  // init plugin_manager
  plugin_manager_ =
      std::make_shared<PluginManager>(xml_path.string(), listPath);

  if (!plugin_manager_->GenerateUnknownTagList()) {
    LOG(ERROR) << "Error during generate unknown list, no exist file, path, "
                  "invalid data format etc, chceck log";
    return Status::ERROR;
  }

  std::vector<std::shared_ptr<PluginInfo>> plugins =
      plugin_manager_->UnknownTagList();

  for (const std::shared_ptr<PluginInfo>& plugin : plugins) {
    if (!plugin_manager_->Launch(plugin->path(), action_type_,
                                 context_->pkgid.get())) {
      LOG(ERROR) << "Error during launch tag name: " << plugin->name()
                 << " path: " << plugin->path();
      return Status::ERROR;
    }
    // add plugin to array for undo process
    if (action_type_ == ActionType::Install) {
      installed_plugins_.push_back(plugin);
    }
  }

  LOG(INFO) << "Successfully processing plugins";
  return Status::OK;
}

Step::Status StepRunParserPlugin::process() {
  return ProcessingPlugins(context_->xml_path.get());
}

Step::Status StepRunParserPlugin::undo() {
  if (action_type_ == ActionType::Install) {
    if (installed_plugins_.empty()) {
      // no installed plugins
      return Status::OK;
    }

    for (const std::shared_ptr<PluginInfo>& plugin : installed_plugins_) {
      if (!plugin_manager_->Launch(plugin->path(), ActionType::Uninstall,
                                   context_->pkgid.get())) {
        LOG(ERROR) << "Error during uninstall tag name: " << plugin->name()
                   << " path: " << plugin->path();
      }
    }
  } else if (action_type_ == ActionType::Upgrade) {
    return ProcessingPlugins(context_->backup_xml_path.get());
  }
  return Status::OK;
}

}  // namespace pkgmgr
}  // namespace common_installer
