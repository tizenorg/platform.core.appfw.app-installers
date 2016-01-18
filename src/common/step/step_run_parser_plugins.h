// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_RUN_PARSER_PLUGINS_H_
#define COMMON_STEP_STEP_RUN_PARSER_PLUGINS_H_

#include <string>
#include <vector>

#include "common/utils/plugin_manager.h"
#include "manifest_parser/utils/logging.h"
#include "common/step/step.h"

namespace common_installer {
namespace pkgmgr {

class StepRunParserPlugin : public Step {
 public:
  explicit StepRunParserPlugin(InstallerContext* context,
                               ActionType action_type);

  Step::Status process() override;
  Step::Status clean() override;
  Step::Status undo() override;
  Step::Status precheck() override;

  SCOPE_LOG_TAG(RunParserPlugin)

 private:
  ActionType action_type_;
  std::vector<std::shared_ptr<PluginInfo>> installed_plugins_;
  std::unique_ptr<PluginManager> plugin_manager_;
  std::string pkgid_;
};

}  // namespace pkgmgr
}  // namespace common_installer

#endif  //  COMMON_STEP_STEP_RUN_PARSER_PLUGINS_H_
