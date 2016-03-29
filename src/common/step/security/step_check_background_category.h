// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_STEP_SECURITY_STEP_CHECK_BACKGROUND_CATEGORY_H_
#define COMMON_STEP_SECURITY_STEP_CHECK_BACKGROUND_CATEGORY_H_

#include <manifest_parser/utils/logging.h>
#include <manifest_parser/utils/version_number.h>

#include <glib.h>
#include <set>
#include <string>

#include "common/step/step.h"

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

  __attribute__ ((visibility ("default"))) explicit StepCheckBackgroundCategory(
      common_installer::InstallerContext* context);

  /**
   * \brief Check background category values
   *
   * \return Status::ERROR when "system" detected,
   *         Status::OK otherwise
   */
  __attribute__ ((visibility ("default"))) Status process() override;

  __attribute__ ((visibility ("default"))) Status clean() override { return Status::OK; }
  __attribute__ ((visibility ("default"))) Status undo() override { return Status::OK; }
  __attribute__ ((visibility ("default"))) Status precheck() override { return Status::OK; }

 protected:
  __attribute__ ((visibility ("default"))) virtual bool GetBackgroundSupport() = 0;

 private:
  __attribute__ ((visibility ("default"))) GList* CopyValuesToBackgroundCategory(
      const BackgroundCatSet& values, GList* backgroundCategories) const;
  __attribute__ ((visibility ("default"))) bool IsTrustedCert(common_installer::PrivilegeLevel privilege) const;
  __attribute__ ((visibility ("default"))) bool ShouldSendFail(common_installer::PrivilegeLevel privilege,
                      const BackgroundCatSet& background_categories) const;
  __attribute__ ((visibility ("default"))) bool ShouldSendAll(const utils::VersionNumber& version,
                     bool background_support,
                     common_installer::PrivilegeLevel privilege,
                     const BackgroundCatSet& background_categories) const;
  __attribute__ ((visibility ("default"))) bool ShouldSendSystem(common_installer::PrivilegeLevel privilege,
                        const BackgroundCatSet& background_categories) const;
  __attribute__ ((visibility ("default"))) bool ShouldSendKnown(const utils::VersionNumber& version,
                       bool background_support,
                       common_installer::PrivilegeLevel privilege,
                       const BackgroundCatSet& background_categories) const;
  __attribute__ ((visibility ("default"))) bool ShouldSendUnknown(common_installer::PrivilegeLevel privilege,
                         const BackgroundCatSet& background_categories) const;

  __attribute__ ((visibility ("default"))) common_installer::Step::Status DoSendFail(
      common_installer::PrivilegeLevel privilege,
      const BackgroundCatSet& background_categories) const;
  __attribute__ ((visibility ("default"))) void DoSendAll(const utils::VersionNumber& version,
                 bool background_support,
                 common_installer::PrivilegeLevel privilege,
                 const BackgroundCatSet& background_categories,
                 application_x* app) const;
  __attribute__ ((visibility ("default"))) void DoSendSystem(common_installer::PrivilegeLevel privilege,
                    const BackgroundCatSet& background_categories,
                    application_x* app) const;
  __attribute__ ((visibility ("default"))) void DoSendKnown(const utils::VersionNumber& version,
                   bool background_support,
                   common_installer::PrivilegeLevel privilege,
                   const BackgroundCatSet& background_categories,
                   application_x* app) const;
  __attribute__ ((visibility ("default"))) void DoSendUnknown(common_installer::PrivilegeLevel privilege,
                     const BackgroundCatSet& background_categories,
                     application_x* app) const;

  __attribute__ ((visibility ("default"))) void GetBackgroundCategories(
      const application_x& app,
      BackgroundCatSet* background_categories) const;
  __attribute__ ((visibility ("default"))) void RemoveContextBackgroundCategories(application_x* app) const;

  const BackgroundCatSet known_background_categories_;
  const BackgroundCatSet not_unknown_background_categories_;

  SCOPE_LOG_TAG(CheckBackgroundCategory)
};

}  // namespace security
}  // namespace common_installer

#endif  // COMMON_STEP_SECURITY_STEP_CHECK_BACKGROUND_CATEGORY_H_
