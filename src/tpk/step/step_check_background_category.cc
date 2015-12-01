// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "tpk/step/step_check_background_category.h"

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "common/installer_context.h"
#include "common/utils/glist_range.h"
#include "common/utils/string_util.h"
#include "wgt/wgt_backend_data.h"

namespace {

namespace tpk_sec = tpk::security;
using CompositeSpec =
    common_installer::CompositeSpecification<tpk_sec::BackgroundCatSpecTest>;

/**
 * \brief Implementation of Specification that check version requirement
 */
class VersionAtLeastSpec : public CompositeSpec {
 public:
  explicit VersionAtLeastSpec(const utils::VersionNumber version)
      : version_(version) {}

  bool IsSatisfiedBy(const tpk_sec::BackgroundCatSpecTest &candidate) override {
    return version_ <= candidate.version;
  }

 private:
  const utils::VersionNumber version_;
};

/**
 * \brief Implementation of Specification that check certificate level
 *        requirement
 */
class CertSpec : public CompositeSpec {
 public:
  explicit CertSpec(common_installer::PrivilegeLevel privilege) :
    privilege_(privilege) {}

  bool IsSatisfiedBy(const tpk_sec::BackgroundCatSpecTest &candidate) override {
    return candidate.cert == privilege_;
  }

 private:
  const common_installer::PrivilegeLevel privilege_;
};

/**
 * \brief Implementation of Specification that check background support
 *        requirement
 */
class BackgroundSuptSpec : public CompositeSpec {
 public:
  bool IsSatisfiedBy(const tpk_sec::BackgroundCatSpecTest &candidate) override {
    return candidate.background_supt;
  }
};

/**
 * \brief Implementation of Specification that check if any known values
 *        were parsed in background-category elements
 */
class KnownValueSpec : public CompositeSpec {
 public:
  explicit KnownValueSpec(const tpk_sec::BackgroundCatSet &values) :
      valid_values_(values) {
  }

  bool IsSatisfiedBy(const tpk_sec::BackgroundCatSpecTest &candidate) override {
    tpk_sec::BackgroundCatSet intersect;
    std::set_intersection(valid_values_.begin(), valid_values_.end(),
        candidate.values.begin(), candidate.values.end(),
        std::inserter(intersect, intersect.begin()));
    return !intersect.empty();
  }

 private:
  tpk_sec::BackgroundCatSet valid_values_;
};

/**
 * \brief Implementation of Specification that check if any unknown values
 *        were parsed in background-category elements
 */
class UnknownValueSpec : public CompositeSpec {
 public:
  explicit UnknownValueSpec(const tpk_sec::BackgroundCatSet &values) :
    invalid_values_(values) {
  }

  bool IsSatisfiedBy(const tpk_sec::BackgroundCatSpecTest &candidate) override {
    tpk_sec::BackgroundCatSet diff;
    std::set_difference(candidate.values.begin(), candidate.values.end(),
        invalid_values_.begin(), invalid_values_.end(),
        std::inserter(diff, diff.begin()));
    return !diff.empty();
  }

 private:
  tpk_sec::BackgroundCatSet invalid_values_;
};

/**
 * \brief Implementation of Specification that check if any of parsed values in
 *        background-category elements meet requirement of having "system" value
 */
class SystemValueSpec : public CompositeSpec {
 public:
  bool IsSatisfiedBy(const tpk_sec::BackgroundCatSpecTest &candidate) override {
    return candidate.values.find("system") != candidate.values.end();
  }
};

/**
 * \brief Implementation of Specification that check if no values were parsed
 *        in background-category elements
 */
class EmptyValueSpec : public CompositeSpec {
 public:
  bool IsSatisfiedBy(const tpk_sec::BackgroundCatSpecTest &candidate) override {
    return candidate.values.empty();
  }
};

}  // namespace

namespace tpk {
namespace security {

StepCheckBackgroundCategory::StepCheckBackgroundCategory(
    common_installer::InstallerContext *context) : Step(context),
        known_background_categories_ {
            "media", "download", "background-network",
            "location", "sensor", "iot-communication" },
        not_unknown_background_categories_ {
            "media", "download", "background-network", "location",
            "sensor", "iot-communication", "system" } {
  utils::VersionNumber v24("2.4");
  auto ver24 = std::make_shared<VersionAtLeastSpec>(std::move(v24));
  auto certUntrusted =
      std::make_shared<CertSpec>(common_installer::PrivilegeLevel::UNTRUSTED);
  auto certPublic =
      std::make_shared<CertSpec>(common_installer::PrivilegeLevel::PUBLIC);
  auto certParter =
      std::make_shared<CertSpec>(common_installer::PrivilegeLevel::PARTNER);
  auto certPlatform =
      std::make_shared<CertSpec>(common_installer::PrivilegeLevel::PLATFORM);
  auto background_supt = std::make_shared<BackgroundSuptSpec>();
  auto known = std::make_shared<KnownValueSpec>(known_background_categories_);
  auto unknown =
      std::make_shared<UnknownValueSpec>(not_unknown_background_categories_);
  auto system = std::make_shared<SystemValueSpec>();
  auto empty = std::make_shared<EmptyValueSpec>();

  common_installer::ISpecPtr<BackgroundCatSpecTest> certTrusted =
      certParter->Or(certPlatform);
  sentAllSpec_ = ver24->Not()->And(
      background_supt->And(certPublic->And(system->Not())->Or(certTrusted)));
  failSpec_ = certPublic->And(system)->Or(certUntrusted);
  systemSpec_ = certTrusted->And(system);
  knownSpec_ = failSpec_->Not()->And(
      sentAllSpec_->Not())->And(empty->Not())->And(known);
  unknownSpec_ = failSpec_->Not()->And(empty->Not())->And(unknown);
}

GList *StepCheckBackgroundCategory::CopyValuesToBackgroundCategory(
    BackgroundCatSet values, GList *backgroundCategories) {
  for (auto& background_category : values) {
    backgroundCategories = g_list_append(
        backgroundCategories, strdup(background_category.c_str()));
  }

  return backgroundCategories;
}

common_installer::Step::Status StepCheckBackgroundCategory::process() {
  std::string str_ver(context_->manifest_data.get()->api_version);
  common_installer::PrivilegeLevel privilege_level =
      context_->privilege_level.get();
  bool bkgnd_supt = true;
  utils::VersionNumber version(str_ver);

  for (application_x* app :
      GListRange<application_x*>(context_->manifest_data.get()->application)) {
    BackgroundCatSet bkgnd_cat;

    for (const char* background_category : GListRange<char*>(
        app->background_category)) {
      bkgnd_cat.insert(strdup(background_category));
    }

    g_list_free_full(app->background_category, g_free);
    app->background_category = nullptr;

    BackgroundCatSpecTest test =
        { version, privilege_level, bkgnd_supt, std::move(bkgnd_cat) };

    if (failSpec_->IsSatisfiedBy(test)) {
      LOG(DEBUG) << "Installation fail caused by background-category";
      return Status::ERROR;
    }

    if (sentAllSpec_->IsSatisfiedBy(test)) {
      app->background_category = CopyValuesToBackgroundCategory(
          known_background_categories_, app->background_category);
    }

    if (systemSpec_->IsSatisfiedBy(test)) {
      app->background_category = g_list_append(
          app->background_category, strdup("system"));
    }

    if (knownSpec_->IsSatisfiedBy(test)) {
      BackgroundCatSet to_insert;

      // Get all known parsed values this is to handle case when more than one
      // background-category element is declared and values are mixed
      std::set_intersection(
          known_background_categories_.begin(),
          known_background_categories_.end(),
          test.values.begin(), test.values.end(),
          std::inserter(to_insert, to_insert.begin()));

      app->background_category = CopyValuesToBackgroundCategory(
          to_insert, app->background_category);
    }

    if (unknownSpec_->IsSatisfiedBy(test)) {
      BackgroundCatSet to_insert;

      // Get all unknown parsed values this is to handle case when more than one
      // background-category element is declared and values are mixed
      std::set_difference(
          test.values.begin(), test.values.end(),
          not_unknown_background_categories_.begin(),
          not_unknown_background_categories_.end(),
          std::inserter(to_insert, to_insert.end()));

      app->background_category = CopyValuesToBackgroundCategory(
          to_insert, app->background_category);
    }
  }

  return Status::OK;
}

}  // namespace security
}  // namespace tpk
