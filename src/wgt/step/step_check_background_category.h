// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_STEP_STEP_CHECK_BACKGROUND_CATEGORY_H_
#define WGT_STEP_STEP_CHECK_BACKGROUND_CATEGORY_H_

#include <glib.h>
#include <set>

#include <manifest_parser/utils/version_number.h>

#include "common/step/step.h"
#include "common/utils/logging.h"
#include "common/utils/specification.h"

namespace wgt {
namespace security {

typedef std::set<std::string> BackgroundCatSet;

struct BackgroundCatSpecTest {
  utils::VersionNumber version;
  common_installer::PrivilegeLevel cert;
  bool background_supt;
  BackgroundCatSet values;
};

/**
 * \brief This step check background category value and modify it depending on
 *        required version, cert level, background support, and value itself
 */
class StepCheckBackgroundCategory : public common_installer::Step {
 public:
  using Step::Step;

  explicit StepCheckBackgroundCategory(
      common_installer::InstallerContext *context);

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
  GList *CopyValuesToBackgroundCategory(
      BackgroundCatSet values, GList *backgroundCategories);

  common_installer::ISpecPtr<BackgroundCatSpecTest> sentAllSpec_;
  common_installer::ISpecPtr<BackgroundCatSpecTest> failSpec_;
  common_installer::ISpecPtr<BackgroundCatSpecTest> systemSpec_;
  common_installer::ISpecPtr<BackgroundCatSpecTest> knownSpec_;
  common_installer::ISpecPtr<BackgroundCatSpecTest> unknownSpec_;

  const BackgroundCatSet known_background_categories_;
  const BackgroundCatSet not_unknown_background_categories_;
};

}  // namespace security
}  // namespace wgt

#endif  // WGT_STEP_STEP_CHECK_BACKGROUND_CATEGORY_H_
