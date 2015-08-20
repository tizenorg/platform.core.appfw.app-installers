// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef TPK_STEP_STEP_PARSE_H_
#define TPK_STEP_STEP_PARSE_H_

#include <boost/filesystem.hpp>
#include <tpk_manifest_handlers/privileges_handler.h>
#include <tpk_manifest_handlers/tpk_config_parser.h>

#include <memory>
#include <set>
#include <string>

#include "common/app_installer.h"
#include "common/context_installer.h"
#include "common/step/step.h"
#include "common/utils/logging.h"

namespace tpk {
namespace parse {

class StepParse : public common_installer::Step {
 public:
  using Step::Step;
  // explicit StepParse(common_installer::ContextInstaller* context,
  //     bool check_start_file);

  Status process() override { return Status::OK; }
  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override { return Status::OK; }

 protected:
  // virtual bool LocateConfigFile();
  bool Check(const boost::filesystem::path& widget_path);
  // required by step_recovery
  virtual boost::filesystem::path LocateConfigFile() const;

  boost::filesystem::path config_;

 private:
  std::set<std::string> ExtractPrivileges(
      std::shared_ptr<const PrivilegesInfo> perm_info) const;

  bool FillIconPaths(manifest_x* manifest);
  bool FillApplicationInfo(manifest_x* manifest);
  bool FillAuthorInfo(manifest_x* manifest);
  bool FillDescription(manifest_x* manifest);
  bool FillServiceApplication(manifest_x* manifest);
  bool FillUIApplication(manifest_x* manifest);
  bool FillAppControl(manifest_x* manifest);
  bool FillDataControl(manifest_x* manifest);
  bool FillLabel(manifest_x* manifest);
  bool FillPrivileges(manifest_x* manifest);
  bool FillMetadata(manifest_x* manifest);
  bool FillAccounts(manifest_x* manifest);
  bool FillManifestX(manifest_x* manifest);

  std::unique_ptr<tpk::parse::TPKConfigParser> parser_;
  bool check_start_file_;

  SCOPE_LOG_TAG(Parse)
};

}  // namespace parse
}  // namespace tpk

#endif  // TPK_STEP_STEP_PARSE_H_
