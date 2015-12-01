// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef TPK_STEP_STEP_PARSE_H_
#define TPK_STEP_STEP_PARSE_H_

#include <boost/filesystem.hpp>

#include <manifest_parser/utils/logging.h>
#include <tpk_manifest_handlers/privileges_handler.h>
#include <tpk_manifest_handlers/tpk_config_parser.h>
#include <tpk_manifest_handlers/ui_and_service_application_infos.h>

#include <memory>
#include <set>
#include <string>

#include "common/app_installer.h"
#include "common/installer_context.h"
#include "common/step/step.h"

namespace tpk {
namespace parse {

class StepParse : public common_installer::Step {
 public:
  using Step::Step;
  explicit StepParse(common_installer::InstallerContext* context,
      bool check_start_file);

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override;

 protected:
  virtual bool LocateConfigFile();
  // This function is needed by recovery mode to override searching
  // of configuration file of the package
  virtual boost::filesystem::path LocateConfigFile() const;
  boost::filesystem::path path_;

 private:
  bool FillPackageInfo(manifest_x* manifest);
  bool FillAuthorInfo(manifest_x* manifest);
  bool FillDescription(manifest_x* manifest);
  bool FillPrivileges(manifest_x* manifest);
  bool FillServiceApplication(manifest_x* manifest);
  bool FillUIApplication(manifest_x* manifest);
  template <typename T>
      bool FillAppControl(application_x* manifest, const T& app_control_list);
  template <typename T>
      bool FillDataControl(application_x* manifest, const T& data_control_list);
  template <typename T>
      bool FillApplicationIconPaths(application_x* manifest,
                                    const T& icons_info);
  template <typename T>
      bool FillLabel(application_x* manifest, const T& label_list);
  template <typename T>
      bool FillMetadata(application_x* manifest, const T& meta_data_list);
  bool FillImage(application_x* app,
                 const tpk::parse::ApplicationImagesInfo& label_list);
  bool FillAccounts();
  bool FillShortcuts();
  template <typename T>
  bool FillBackgroundCategoryInfo(application_x* app,
      const T& background_category_data_list);
  bool FillManifestX(manifest_x* manifest);

  std::unique_ptr<tpk::parse::TPKConfigParser> parser_;

  SCOPE_LOG_TAG(Parse)
};

}  // namespace parse
}  // namespace tpk

#endif  // TPK_STEP_STEP_PARSE_H_
