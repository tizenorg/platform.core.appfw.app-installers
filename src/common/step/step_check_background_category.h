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

namespace ci = common_installer;

typedef std::set<std::string> BackgroundCatSet;

/**
 * \brief This step check background category value and modify it depending on
 *        required version, cert level, background support, and value itself
 */
class BaseStepCheckBackgroundCategory : public ci::Step {
 public:
  using Step::Step;

  explicit BaseStepCheckBackgroundCategory(ci::InstallerContext * context);

  /**
   * \brief Check background category values
   *
   * \return Status::ERROR when "system" detected,
   *         Status::OK otherwise
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

  SCOPE_LOG_TAG(CheckBackgroundCategory)

 private:
  GList * CopyValuesToBackgroundCategory(
      BackgroundCatSet const& values, GList * backgroundCategories);
  bool IsTrustedCert(ci::PrivilegeLevel privilege);
  bool ShouldSendFail(ci::PrivilegeLevel privilege,
                      BackgroundCatSet const& background_categories);
  bool ShouldSendAll(utils::VersionNumber const& version,
                     bool background_support,
                     ci::PrivilegeLevel privilege,
                     BackgroundCatSet const& background_categories);
  bool ShouldSendSystem(ci::PrivilegeLevel privilege,
                        BackgroundCatSet const& background_categories);
  bool ShouldSendKnown(utils::VersionNumber const& version,
                       bool background_support,
                       ci::PrivilegeLevel privilege,
                       BackgroundCatSet const& background_categories);
  bool ShouldSendUnknown(ci::PrivilegeLevel privilege,
                         BackgroundCatSet const& background_categories);

  ci::Step::Status DoSendFail(ci::PrivilegeLevel privilege,
                              BackgroundCatSet const& background_categories);
  void DoSendAll(utils::VersionNumber const& version,
                 bool background_support,
                 ci::PrivilegeLevel privilege,
                 BackgroundCatSet const& background_categories,
                 application_x * app);
  void DoSendSystem(ci::PrivilegeLevel privilege,
                    BackgroundCatSet const& background_categories,
                    application_x * app);
  void DoSendKnown(utils::VersionNumber const& version,
                   bool background_support,
                   ci::PrivilegeLevel privilege,
                   BackgroundCatSet const& background_categories,
                   application_x * app);
  void DoSendUnknown(ci::PrivilegeLevel privilege,
                     BackgroundCatSet const& background_categories,
                     application_x * app);

  void GetBackgroundCategories(
      application_x const& app,
      BackgroundCatSet * background_categories);
  void RemoveContextBackgroundCategories(application_x * app);

  const BackgroundCatSet known_background_categories_;
  const BackgroundCatSet not_unknown_background_categories_;

 protected:
  virtual bool GetBackgroundSupport() = 0;
};

}  // namespace security
}  // namespace common

#endif  // COMMON_STEP_STEP_CHECK_BACKGROUND_CATEGORY_H_
