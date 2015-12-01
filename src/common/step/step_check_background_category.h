// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_STEP_CHECK_BACKGROUND_CATEGORY_H_
#define COMMON_STEP_STEP_CHECK_BACKGROUND_CATEGORY_H_

#include <manifest_parser/utils/version_number.h>

#include <glib.h>
#include <set>
#include <string>

#include "common/step/step.h"
#include "common/utils/logging.h"

namespace common_installer {
namespace security {

typedef std::set<std::string> BackgroundCatSet;

/**
 * \brief This step check background category value and modify it depending on
 *        required version, cert level, background support, and value itself
 */
class StepCheckBackgroundCategory : public common_installer::Step {
 public:
  using Step::Step;

  explicit StepCheckBackgroundCategory(
      common_installer::InstallerContext* context);

  /**
   * \brief Check background category values
   *
   * \return Status::ERROR when "system" detected,
   *         Status::OK otherwise
   */
  Status process() override;

  Status clean() override { return Status::OK; }
  Status undo() override { return Status::OK; }
  Status precheck() override { return Status::OK; }

 protected:
  virtual bool GetBackgroundSupport() = 0;

 private:
  GList* CopyValuesToBackgroundCategory(
      const BackgroundCatSet& values, GList* backgroundCategories) const;
  bool IsTrustedCert(common_installer::PrivilegeLevel privilege) const;
  bool ShouldSendFail(common_installer::PrivilegeLevel privilege,
                      const BackgroundCatSet& background_categories) const;
  bool ShouldSendAll(const utils::VersionNumber& version,
                     bool background_support,
                     common_installer::PrivilegeLevel privilege,
                     const BackgroundCatSet& background_categories) const;
  bool ShouldSendSystem(common_installer::PrivilegeLevel privilege,
                        const BackgroundCatSet& background_categories) const;
  bool ShouldSendKnown(const utils::VersionNumber& version,
                       bool background_support,
                       common_installer::PrivilegeLevel privilege,
                       const BackgroundCatSet& background_categories) const;
  bool ShouldSendUnknown(common_installer::PrivilegeLevel privilege,
                         const BackgroundCatSet& background_categories) const;

  common_installer::Step::Status DoSendFail(
      common_installer::PrivilegeLevel privilege,
      const BackgroundCatSet& background_categories) const;
  void DoSendAll(const utils::VersionNumber& version,
                 bool background_support,
                 common_installer::PrivilegeLevel privilege,
                 const BackgroundCatSet& background_categories,
                 application_x* app) const;
  void DoSendSystem(common_installer::PrivilegeLevel privilege,
                    const BackgroundCatSet& background_categories,
                    application_x* app) const;
  void DoSendKnown(const utils::VersionNumber& version,
                   bool background_support,
                   common_installer::PrivilegeLevel privilege,
                   const BackgroundCatSet& background_categories,
                   application_x* app) const;
  void DoSendUnknown(common_installer::PrivilegeLevel privilege,
                     const BackgroundCatSet& background_categories,
                     application_x* app) const;

  void GetBackgroundCategories(
      const application_x& app,
      BackgroundCatSet* background_categories) const;
  void RemoveContextBackgroundCategories(application_x* app) const;

  const BackgroundCatSet known_background_categories_;
  const BackgroundCatSet not_unknown_background_categories_;

  SCOPE_LOG_TAG(CheckBackgroundCategory)
};

}  // namespace security
}  // namespace common_installer

#endif  // COMMON_STEP_STEP_CHECK_BACKGROUND_CATEGORY_H_
