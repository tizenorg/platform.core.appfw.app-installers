// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_check_background_category.h"

#include <algorithm>

#include "common/installer_context.h"
#include "common/utils/glist_range.h"

namespace common_installer {
namespace security {

BaseStepCheckBackgroundCategory::BaseStepCheckBackgroundCategory(
    common_installer::InstallerContext *context) : Step(context),
        known_background_categories_ {
            "media", "download", "background-network",
            "location", "sensor", "iot-communication" },
        not_unknown_background_categories_ {
            "media", "download", "background-network", "location",
            "sensor", "iot-communication", "system" } {
}

GList *BaseStepCheckBackgroundCategory::CopyValuesToBackgroundCategory(
    BackgroundCatSet values, GList *backgroundCategories) {
  for (const auto& background_category : values) {
    backgroundCategories = g_list_append(
        backgroundCategories, strdup(background_category.c_str()));
  }

  return backgroundCategories;
}

bool BaseStepCheckBackgroundCategory::IsTrustedCert(
    ci::PrivilegeLevel privilege) {
  return privilege == ci::PrivilegeLevel::PARTNER ||
      privilege == ci::PrivilegeLevel::PLATFORM;
}

bool BaseStepCheckBackgroundCategory::ShouldSendFail(
    ci::PrivilegeLevel privilege,
    BackgroundCatSet & background_categories) {
  return (privilege == ci::PrivilegeLevel::PUBLIC &&
      background_categories.find("system") != background_categories.end()) ||
      (privilege == ci::PrivilegeLevel::UNTRUSTED &&
          !background_categories.empty());
}

bool BaseStepCheckBackgroundCategory::ShouldSendAll(
    utils::VersionNumber & version,
    bool background_support,
    ci::PrivilegeLevel privilege,
    BackgroundCatSet & background_categories) {
  utils::VersionNumber ver24("2.4");
  return version < ver24 && background_support &&
      ((privilege == ci::PrivilegeLevel::PUBLIC &&
          background_categories.find("system") ==
              background_categories.end()) || IsTrustedCert(privilege));
}

bool BaseStepCheckBackgroundCategory::ShouldSendSystem(
    ci::PrivilegeLevel privilege,
    BackgroundCatSet & background_categories) {
  return IsTrustedCert(privilege) &&
      background_categories.find("system") != background_categories.end();
}

bool BaseStepCheckBackgroundCategory::ShouldSendKnown(
    utils::VersionNumber & version,
    bool background_support,
    ci::PrivilegeLevel privilege,
    BackgroundCatSet & background_categories) {
  BackgroundCatSet intersect;
  std::set_intersection(known_background_categories_.begin(),
      known_background_categories_.end(),
      background_categories.begin(), background_categories.end(),
      std::inserter(intersect, intersect.begin()));

  return !(ShouldSendFail(privilege, background_categories) ||
      ShouldSendAll(
          version, background_support, privilege, background_categories)) &&
      !background_categories.empty() && !intersect.empty();
}

bool BaseStepCheckBackgroundCategory::ShouldSendUnknown(
    ci::PrivilegeLevel privilege,
    BackgroundCatSet & background_categories) {
  BackgroundCatSet diff;
  std::set_difference(background_categories.begin(),
      background_categories.end(),
      not_unknown_background_categories_.begin(),
      not_unknown_background_categories_.end(),
      std::inserter(diff, diff.begin()));

  return !(ShouldSendFail(privilege, background_categories) ||
      background_categories.empty() || diff.empty());
}

common_installer::Step::Status BaseStepCheckBackgroundCategory::process() {
  std::string str_ver(context_->manifest_data.get()->api_version);
  common_installer::PrivilegeLevel privilege_level =
      context_->privilege_level.get();
  bool background_supt = GetBackgroundSupport();
  utils::VersionNumber version(str_ver);

  for (application_x* app :
      GListRange<application_x*>(context_->manifest_data.get()->application)) {
    BackgroundCatSet background_cat;

    for (const char* background_category : GListRange<char*>(
        app->background_category)) {
      background_cat.insert(background_category);
    }

    g_list_free_full(app->background_category, g_free);
    app->background_category = nullptr;

    if (ShouldSendFail(privilege_level, background_cat)) {
      LOG(DEBUG) << "Installation fail caused by background-category";
      return Status::ERROR;
    }

    if (ShouldSendAll(version, background_supt, privilege_level,
        background_cat)) {
      app->background_category = CopyValuesToBackgroundCategory(
          known_background_categories_, app->background_category);
    }

    if (ShouldSendSystem(privilege_level, background_cat)) {
      app->background_category = g_list_append(
          app->background_category, strdup("system"));
    }

    if (ShouldSendKnown(version, background_supt, privilege_level,
        background_cat)) {
      BackgroundCatSet to_insert;

      // Get all known parsed values this is to handle case when more than one
      // background-category element is declared and values are mixed
      std::set_intersection(
          known_background_categories_.begin(),
          known_background_categories_.end(),
          background_cat.begin(), background_cat.end(),
          std::inserter(to_insert, to_insert.begin()));

      app->background_category = CopyValuesToBackgroundCategory(
          to_insert, app->background_category);
    }

    if (ShouldSendUnknown(privilege_level, background_cat)) {
      BackgroundCatSet to_insert;

      // Get all unknown parsed values this is to handle case when more than one
      // background-category element is declared and values are mixed
      std::set_difference(
          background_cat.begin(), background_cat.end(),
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
}  // namespace common
