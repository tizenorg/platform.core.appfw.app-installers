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

Step::Status StepRunParserPlugin::process() {
  const boost::filesystem::path xmlPath(context_->xml_path.get());

  pkgid_ = context_->pkgid.get();

  // PLUGINS_LIST_FILE_PATH path generated from cmake
  const std::string listPath(PLUGINS_LIST_FILE_PATH);

  // init plugin_manager
  plugin_manager_ = std::unique_ptr<PluginManager>(
      new PluginManager(xmlPath.string(), listPath));

  if (plugin_manager_->GenerateUnknownTagList()) {
    std::vector<std::shared_ptr<PluginInfo>> plugins =
        plugin_manager_->UnknownTagList();

    for (const std::shared_ptr<PluginInfo>& plugin : plugins) {
      if (!plugin_manager_->Launch(plugin->Path(), action_type_, pkgid_)) {
        LOG(ERROR) << "Error during launch tag name: " << plugin->Name()
                   << " path: " << plugin->Path();
        return Status::ERROR;
      }
      // add plugin to array for undo process
      if (action_type_ == ActionType::Install) {
        installed_plugins_.push_back(plugin);
      }
    }
  } else {
    LOG(ERROR) << "Error during generate unknown list, no exist file, path, "
                  "invalid data format etc, chceck log";
    return Status::ERROR;
  }

  return Status::OK;
}

Step::Status StepRunParserPlugin::clean() {
  // nothing to do
  return Status::OK;
}

Step::Status StepRunParserPlugin::undo() {
  if (installed_plugins_.empty()) {
    // no installed plugins
    return Status::OK;
  }

  for (const std::shared_ptr<PluginInfo>& plugin : installed_plugins_) {
    if (!plugin_manager_->Launch(plugin->Path(), ActionType::Uninstall,
                                 pkgid_)) {
      LOG(ERROR) << "Error during uninstall tag name: " << plugin->Name()
                 << " path: " << plugin->Path();
      return Status::ERROR;
    }
  }

  return Status::OK;
}

Step::Status StepRunParserPlugin::precheck() {
  // nothing to do
  return Status::OK;
}

}  // namespace pkgmgr
}  // namespace common_installer
