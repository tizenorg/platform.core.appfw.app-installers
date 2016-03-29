// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_PKGMGR_STEP_RUN_PARSER_PLUGINS_H_
#define COMMON_STEP_PKGMGR_STEP_RUN_PARSER_PLUGINS_H_

#include <manifest_parser/utils/logging.h>

#include <pkgmgrinfo_basic.h>

#include <string>
#include <vector>

#include "common/plugins/plugin.h"
#include "common/step/step.h"

namespace common_installer {
namespace pkgmgr {

class StepRunParserPlugin : public Step {
 public:
  __attribute__ ((visibility ("default"))) explicit StepRunParserPlugin(InstallerContext* context,
                               Plugin::ActionType action_type);

  __attribute__ ((visibility ("default"))) Step::Status process() override;
  __attribute__ ((visibility ("default"))) Step::Status clean() { return Status::OK; }
  __attribute__ ((visibility ("default"))) Step::Status undo() override;
  __attribute__ ((visibility ("default"))) Step::Status precheck() { return Status::OK; }

 private:
  __attribute__ ((visibility ("default"))) Step::Status ProcessPlugins(const boost::filesystem::path& xml_path,
                              manifest_x* manifest,
                              Plugin::ActionType action_type);

  Plugin::ActionType action_type_;

  SCOPE_LOG_TAG(RunParserPlugin)
};

}  // namespace pkgmgr
}  // namespace common_installer

#endif  //  COMMON_STEP_PKGMGR_STEP_RUN_PARSER_PLUGINS_H_
