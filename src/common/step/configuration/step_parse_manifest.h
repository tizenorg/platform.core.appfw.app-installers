// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_CONFIGURATION_STEP_PARSE_MANIFEST_H_
#define COMMON_STEP_CONFIGURATION_STEP_PARSE_MANIFEST_H_

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
namespace configuration {

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

  __attribute__ ((visibility ("default"))) explicit StepParseManifest(common_installer::InstallerContext* context,
      ManifestLocation manifest_location, StoreLocation store_location);

  __attribute__ ((visibility ("default"))) Status process() override;
  __attribute__ ((visibility ("default"))) Status clean() override { return Status::OK; }
  __attribute__ ((visibility ("default"))) Status undo() override { return Status::OK; }
  __attribute__ ((visibility ("default"))) Status precheck() override;

 protected:
  __attribute__ ((visibility ("default"))) bool LocateConfigFile();
  boost::filesystem::path path_;

 private:
  __attribute__ ((visibility ("default"))) bool FillInstallationInfo(manifest_x* manifest);
  __attribute__ ((visibility ("default"))) bool FillPackageInfo(manifest_x* manifest);
  __attribute__ ((visibility ("default"))) bool FillAuthorInfo(manifest_x* manifest);
  __attribute__ ((visibility ("default"))) bool FillDescriptionInfo(manifest_x* manifest);
  __attribute__ ((visibility ("default"))) bool FillPrivileges(manifest_x* manifest);
  __attribute__ ((visibility ("default"))) bool FillWidgetApplication(manifest_x* manifest);
  __attribute__ ((visibility ("default"))) bool FillServiceApplication(manifest_x* manifest);
  __attribute__ ((visibility ("default"))) bool FillUIApplication(manifest_x* manifest);
  __attribute__ ((visibility ("default"))) bool FillWatchApplication(manifest_x* manifest);

  template <typename T>
      __attribute__ ((visibility ("default"))) bool FillAppControl(application_x* manifest, const T& app_control_list);
  template <typename T>
      __attribute__ ((visibility ("default"))) bool FillDataControl(application_x* manifest, const T& data_control_list);
  template <typename T>
      __attribute__ ((visibility ("default"))) bool FillApplicationIconPaths(application_x* manifest,
                                    const T& icons_info);
  template <typename T>
      __attribute__ ((visibility ("default"))) bool FillLabel(application_x* manifest, const T& label_list);
  template <typename T>
      __attribute__ ((visibility ("default"))) bool FillMetadata(application_x* manifest, const T& meta_data_list);
  template <typename T>
      __attribute__ ((visibility ("default"))) bool FillCategories(application_x* manifest, const T& meta_data_list);
  __attribute__ ((visibility ("default"))) bool FillImage(application_x* app,
                 const tpk::parse::ApplicationImagesInfo& label_list);
  __attribute__ ((visibility ("default"))) bool FillAccounts();
  __attribute__ ((visibility ("default"))) bool FillShortcuts();
  template <typename T>
  __attribute__ ((visibility ("default"))) bool FillBackgroundCategoryInfo(application_x* app,
      const T& background_category_data_list);
  template <typename T>
  __attribute__ ((visibility ("default"))) bool FillSplashScreen(application_x* app,
      const T& splashscreen_list);
  __attribute__ ((visibility ("default"))) bool FillManifestX(manifest_x* manifest);

  std::unique_ptr<tpk::parse::TPKConfigParser> parser_;
  ManifestLocation manifest_location_;
  StoreLocation store_location_;

  SCOPE_LOG_TAG(ParseManifest)
};

}  // namespace configuration
}  // namespace common_installer

#endif  // COMMON_STEP_CONFIGURATION_STEP_PARSE_MANIFEST_H_
