// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_PARSE_MANIFEST_H_
#define COMMON_STEP_STEP_PARSE_MANIFEST_H_

#include <boost/filesystem.hpp>

#include <common/app_installer.h>
#include <common/installer_context.h>
#include <common/step/step.h>

#include <manifest_parser/utils/logging.h>
#include <tpk_manifest_handlers/privileges_handler.h>
#include <tpk_manifest_handlers/tpk_config_parser.h>
#include <tpk_manifest_handlers/ui_and_service_application_infos.h>

#include <memory>
#include <set>
#include <string>

namespace common_installer {
namespace parse {

/**
 * @brief The StepParseManifest class
 *        Used to parse tpk manifest file and store information to context
 *
 * This step is parameterized according to:
 *  - where to look for manifest file
 *  - where to store information from manifest in context structure
 *
 * Different request mode will choose different ManifestLocation and
 * StoreLocation for its purpose but main goal of this step parsing tpk
 * manifest doesn't change.
 */
class StepParseManifest : public common_installer::Step {
 public:
  enum class ManifestLocation {
    PACKAGE,    // parse manifest file from unpacking diretory
    INSTALLED,  // parse manfiest file from current package installation
    RECOVERY    // parse manifest file from backup location or package location
  };

  enum class StoreLocation {
    NORMAL,  // store in context as current application information (new)
    BACKUP   // store in context as old version application information (update)
  };

  explicit StepParseManifest(common_installer::InstallerContext* context,
      ManifestLocation manifest_location, StoreLocation store_location);

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override;

 protected:
  bool LocateConfigFile();
  boost::filesystem::path path_;

 private:
  bool FillInstallationInfo(manifest_x* manifest);
  bool FillPackageInfo(manifest_x* manifest);
  bool FillAuthorInfo(manifest_x* manifest);
  bool FillDescription(manifest_x* manifest);
  bool FillPrivileges(manifest_x* manifest);
  bool FillWidgetApplication(manifest_x* manifest);
  bool FillServiceApplication(manifest_x* manifest);
  bool FillUIApplication(manifest_x* manifest);
  bool FillWatchApplication(manifest_x* manifest);

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
  template <typename T>
      bool FillCategories(application_x* manifest, const T& meta_data_list);
  bool FillImage(application_x* app,
                 const tpk::parse::ApplicationImagesInfo& label_list);
  bool FillAccounts();
  bool FillShortcuts();
  template <typename T>
  bool FillBackgroundCategoryInfo(application_x* app,
      const T& background_category_data_list);
  template <typename T>
  bool FillSplashScreen(application_x* app,
      const T& splashscreen_list);
  bool FillManifestX(manifest_x* manifest);

  std::unique_ptr<tpk::parse::TPKConfigParser> parser_;
  ManifestLocation manifest_location_;
  StoreLocation store_location_;

  SCOPE_LOG_TAG(ParseManifest)
};

}  // namespace parse
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_PARSE_MANIFEST_H_
