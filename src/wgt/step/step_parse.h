// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_STEP_PARSE_H_
#define WGT_STEP_STEP_PARSE_H_

#include <boost/filesystem.hpp>
#include <manifest_handlers/permissions_handler.h>
#include <manifest_handlers/widget_config_parser.h>

#include <memory>
#include <set>
#include <string>

#include "common/app_installer.h"
#include "common/installer_context.h"
#include "common/step/step.h"
#include "common/utils/logging.h"

namespace wgt {
namespace parse {

/**
 * \brief This step parse manifest.xml document
 */
class StepParse : public common_installer::Step {
 public:

  /**
   * \brief Explicit constructor
   *
   * \param context Installer context
   * \param check_start_file Flag if start file should be validated
   */
  explicit StepParse(common_installer::InstallerContext* context,
      bool check_start_file);

  /**
   * \brief
   */
  Status process() override;

  /**
   * \brief Empty method
   *
   * \return Status::OK
   */
  Status clean() override { return Status::OK; }

  /**
   * \brief Empty method
   *
   * \return Status::OK
   */
  Status undo() override { return Status::OK; }

  /**
   * \brief Empty method
   *
   * \return Status::OK
   */
  Status precheck() override { return Status::OK; }

 protected:
  virtual bool LocateConfigFile();
  bool Check(const boost::filesystem::path& widget_path);

  boost::filesystem::path config_;

 private:
  std::set<std::string> ExtractPrivileges(
      std::shared_ptr<const PermissionsInfo> perm_info) const;

  const std::string& GetPackageVersion(const std::string& manifest_version);

  bool FillIconPaths(manifest_x* manifest);
  bool FillWidgetInfo(manifest_x* manifest);
  bool FillUIApplicationInfo(manifest_x* manifest);
  bool FillServiceApplicationInfo(manifest_x* manifest);
  bool FillAppControl(manifest_x* manifest);
  bool FillPrivileges(manifest_x* manifest);
  bool FillCategories(manifest_x* manifest);
  bool FillMetadata(manifest_x* manifest);
  bool FillExtraManifestInfo(manifest_x* manifest);
  bool FillAccounts(manifest_x* manifest);
  bool FillBackgroundCategoryInfo(manifest_x* manifest);
  bool FillManifestX(manifest_x* manifest);

  std::unique_ptr<wgt::parse::WidgetConfigParser> parser_;
  bool check_start_file_;

  SCOPE_LOG_TAG(Parse)
};

}  // namespace parse
}  // namespace wgt

#endif  // WGT_STEP_STEP_PARSE_H_
