// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_check_background_category.h"

#include <algorithm>

#include "common/installer_context.h"
#include "common/utils/glist_range.h"

namespace {
const utils::VersionNumber ver24("2.4");
}  // namespace

namespace common_installer {
namespace security {

StepCheckBackgroundCategory::StepCheckBackgroundCategory(
    ci::InstallerContext * context) : Step(context),
        known_background_categories_ {
            "media", "download", "background-network",
            "location", "sensor", "iot-communication" },
        not_unknown_background_categories_ {
            "media", "download", "background-network", "location",
            "sensor", "iot-communication", "system" } {
}

GList * StepCheckBackgroundCategory::CopyValuesToBackgroundCategory(
    const BackgroundCatSet & values, GList * backgroundCategories) {
  for (const auto& background_category : values) {
    backgroundCategories = g_list_append(
        backgroundCategories, strdup(background_category.c_str()));
  }

  return backgroundCategories;
}

bool StepCheckBackgroundCategory::IsTrustedCert(
    ci::PrivilegeLevel privilege) {
  return privilege == ci::PrivilegeLevel::PARTNER ||
      privilege == ci::PrivilegeLevel::PLATFORM;
}

bool StepCheckBackgroundCategory::ShouldSendFail(
    ci::PrivilegeLevel privilege,
    const BackgroundCatSet & background_categories) {
  return (privilege == ci::PrivilegeLevel::PUBLIC &&
      background_categories.find("system") != background_categories.end()) ||
      (privilege == ci::PrivilegeLevel::UNTRUSTED &&
          !background_categories.empty());
}

bool StepCheckBackgroundCategory::ShouldSendAll(
    const utils::VersionNumber & version,
    bool background_support,
    ci::PrivilegeLevel privilege,
    const BackgroundCatSet & background_categories) {
  return version < ver24 && background_support &&
      ((privilege == ci::PrivilegeLevel::PUBLIC &&
          background_categories.find("system") ==
              background_categories.end()) || IsTrustedCert(privilege));
}

bool StepCheckBackgroundCategory::ShouldSendSystem(
    ci::PrivilegeLevel privilege,
    const BackgroundCatSet & background_categories) {
  return IsTrustedCert(privilege) &&
      background_categories.find("system") != background_categories.end();
}

bool StepCheckBackgroundCategory::ShouldSendKnown(
    const utils::VersionNumber & version,
    bool background_support,
    ci::PrivilegeLevel privilege,
    const BackgroundCatSet & background_categories) {
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

bool StepCheckBackgroundCategory::ShouldSendUnknown(
    ci::PrivilegeLevel privilege,
    const BackgroundCatSet & background_categories) {
  BackgroundCatSet diff;
  std::set_difference(background_categories.begin(),
      background_categories.end(),
      not_unknown_background_categories_.begin(),
      not_unknown_background_categories_.end(),
      std::inserter(diff, diff.begin()));

  return !(ShouldSendFail(privilege, background_categories) ||
      background_categories.empty() || diff.empty());
}

void StepCheckBackgroundCategory::GetBackgroundCategories(
    application_x const& app,
    BackgroundCatSet * background_categories) {
  for (const char* background_category : GListRange<char*>(
      app.background_category)) {
    background_categories->insert(background_category);
  }
}

ci::Step::Status StepCheckBackgroundCategory::DoSendFail(
    ci::PrivilegeLevel privilege,
    const BackgroundCatSet & background_categories) {
  if (ShouldSendFail(privilege, background_categories)) {
    LOG(ERROR) << "Installation fail caused by background-category";
    return Status::ERROR;
  }

  return Status::OK;
}

void StepCheckBackgroundCategory::DoSendAll(
    const utils::VersionNumber & version,
    bool background_support,
    ci::PrivilegeLevel privilege,
    const BackgroundCatSet & background_categories,
    application_x * app) {
  if (ShouldSendAll(version, background_support, privilege,
      background_categories)) {
    app->background_category = CopyValuesToBackgroundCategory(
        known_background_categories_, app->background_category);
  }
}

void StepCheckBackgroundCategory::DoSendSystem(
    ci::PrivilegeLevel privilege,
    const BackgroundCatSet & background_categories,
    application_x * app) {
  if (ShouldSendSystem(privilege, background_categories)) {
    app->background_category = g_list_append(
        app->background_category, strdup("system"));
  }
}

void StepCheckBackgroundCategory::DoSendKnown(
    const utils::VersionNumber & version,
    bool background_support,
    ci::PrivilegeLevel privilege,
    const BackgroundCatSet & background_categories,
    application_x * app) {
  if (ShouldSendKnown(version, background_support, privilege,
      background_categories)) {
    BackgroundCatSet to_insert;

    // Get all known parsed values this is to handle case when more than one
    // background-category element is declared and values are mixed
    std::set_intersection(
        known_background_categories_.begin(),
        known_background_categories_.end(),
        background_categories.begin(), background_categories.end(),
        std::inserter(to_insert, to_insert.begin()));

    app->background_category = CopyValuesToBackgroundCategory(
        to_insert, app->background_category);
  }
}

void StepCheckBackgroundCategory::DoSendUnknown(
    ci::PrivilegeLevel privilege,
    const BackgroundCatSet & background_categories,
    application_x * app) {
  if (ShouldSendUnknown(privilege, background_categories)) {
    BackgroundCatSet to_insert;

    // Get all unknown parsed values this is to handle case when more than one
    // background-category element is declared and values are mixed
    std::set_difference(
        background_categories.begin(), background_categories.end(),
        not_unknown_background_categories_.begin(),
        not_unknown_background_categories_.end(),
        std::inserter(to_insert, to_insert.end()));

    app->background_category = CopyValuesToBackgroundCategory(
        to_insert, app->background_category);
  }
}

void StepCheckBackgroundCategory::RemoveContextBackgroundCategories(
    application_x * app) {
  g_list_free_full(app->background_category, g_free);
  app->background_category = nullptr;
}

ci::Step::Status StepCheckBackgroundCategory::process() {
  std::string str_ver(context_->manifest_data.get()->api_version);
  ci::PrivilegeLevel privilege_level = context_->privilege_level.get();
  bool background_supt = GetBackgroundSupport();
  utils::VersionNumber version(str_ver);

  for (application_x* app :
      GListRange<application_x*>(context_->manifest_data.get()->application)) {
    BackgroundCatSet background_cat;

    GetBackgroundCategories(*app, &background_cat);
    RemoveContextBackgroundCategories(app);

    if (DoSendFail(privilege_level, background_cat) == ci::Step::Status::ERROR)
      return ci::Step::Status::ERROR;
    DoSendAll(version, background_supt, privilege_level, background_cat, app);
    DoSendSystem(privilege_level, background_cat, app);
    DoSendKnown(version, background_supt, privilege_level, background_cat, app);
    DoSendUnknown(privilege_level, background_cat, app);
  }

  return Status::OK;
}

}  // namespace security
}  // namespace common_installer
