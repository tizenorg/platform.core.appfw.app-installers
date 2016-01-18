// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include <string>
#include <vector>

#include "common/step/step_run_parser_plugins.h"
#include "common/utils/plugin_manager.h"

namespace common_installer {
namespace pkgmgr {

Step::Status StepRunParserPlugin::process() {
  const boost::filesystem::path xmlPath(context_->xml_path.get());

  std::string pkgid(context_->pkgid.get());

  // PLUGINS_LIST_FILE_PATH path generated from cmake
  const std::string listPath(PLUGINS_LIST_FILE_PATH);

  std::shared_ptr<PluginManager> plugin_manager =
      std::make_shared<PluginManager>(xmlPath.string(), listPath);

  if (plugin_manager->Generate()) {
    std::vector<std::shared_ptr<PluginInfo>> plugins =
        plugin_manager->UnknownTagList();

    for (std::shared_ptr<PluginInfo> plugin : plugins) {
      // run plugin launcher plugin->Path(), plugin_manager->DocPtr()
    }
  }

  return Status::OK;
}

Step::Status StepRunParserPlugin::clean() { return Status::OK; }

Step::Status StepRunParserPlugin::undo() { return Status::OK; }

Step::Status StepRunParserPlugin::precheck() { return Status::OK; }

}  // namespace pkgmgr
}  // namespace common_installer
