// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_check_background_category.h"

#include <algorithm>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <manifest_handlers/setting_handler.h>

#include "common/installer_context.h"
#include "common/utils/glist_range.h"
#include "common/utils/logging.h"
#include "common/utils/specification.h"
#include "common/utils/string_util.h"
#include "wgt/wgt_backend_data.h"

namespace {

const std::set<std::string> known_background_categories = {
    "media", "download", "background-network", "location",
    "sensor", "iot-communication" };
const std::set<std::string> not_unknown_background_categories = {
    "media", "download", "background-network", "location",
    "sensor", "iot-communication", "system" };

struct BackgroundCatSpecTest {
  std::string version;
  common_installer::PrivilegeLevel cert;
  bool background_supt;
  std::set<std::string> values;
};

class VersionAtLeastSpec :
    public common_installer::CompositeSpecification<BackgroundCatSpecTest> {
 public:
  explicit VersionAtLeastSpec(const std::string version) : version_(version) {}

  bool IsSatisfiedBy(const BackgroundCatSpecTest &candidate) override {
    return common_installer::CompareVersion(candidate.version, version_) >= 0;
  }

 private:
  const std::string version_;
};

class CertSpec :
    public common_installer::CompositeSpecification<BackgroundCatSpecTest> {
 public:
  explicit CertSpec(common_installer::PrivilegeLevel privilege) :
    privilege_(privilege) {}

  bool IsSatisfiedBy(const BackgroundCatSpecTest &candidate) override {
    return candidate.cert == privilege_;
  }

 private:
  const common_installer::PrivilegeLevel privilege_;
};

class BackgroundSuptSpec :
    public common_installer::CompositeSpecification<BackgroundCatSpecTest> {
 public:
  bool IsSatisfiedBy(const BackgroundCatSpecTest &candidate) override {
    return candidate.background_supt;
  }
};

class KnownValueSpec :
    public common_installer::CompositeSpecification<BackgroundCatSpecTest> {
 public:
  explicit KnownValueSpec(const std::set<std::string> &values) :
      valid_values_(values) {
  }

  bool IsSatisfiedBy(const BackgroundCatSpecTest &candidate) override {
    std::set<std::string> intersect;
    std::set_intersection(valid_values_.begin(), valid_values_.end(),
        candidate.values.begin(), candidate.values.end(),
        std::inserter(intersect, intersect.begin()));
    return !intersect.empty();
  }

 private:
  std::set<std::string> valid_values_;
};

class UnknownValueSpec :
    public common_installer::CompositeSpecification<BackgroundCatSpecTest> {
 public:
  explicit UnknownValueSpec(const std::set<std::string> &values) :
    invalid_values_(values) {
  }

  bool IsSatisfiedBy(const BackgroundCatSpecTest &candidate) override {
    std::set<std::string> diff;
    std::set_difference(candidate.values.begin(), candidate.values.end(),
        invalid_values_.begin(), invalid_values_.end(),
        std::inserter(diff, diff.begin()));
    return !diff.empty();
  }

 private:
  std::set<std::string> invalid_values_;
};

class SystemValueSpec :
    public common_installer::CompositeSpecification<BackgroundCatSpecTest> {
 public:
  bool IsSatisfiedBy(const BackgroundCatSpecTest &candidate) override {
    return candidate.values.find("system") != candidate.values.end();
  }
};

class EmptyValueSpec :
    public common_installer::CompositeSpecification<BackgroundCatSpecTest> {
 public:
  bool IsSatisfiedBy(const BackgroundCatSpecTest &candidate) override {
    return candidate.values.empty();
  }
};

}  // namespace

namespace wgt {
namespace security {

common_installer::Step::Status StepCheckBackgroundCategory::process() {
  std::shared_ptr<VersionAtLeastSpec> ver24 =
      std::make_shared<VersionAtLeastSpec>("2.4");
  std::shared_ptr<CertSpec> certUntrusted =
      std::make_shared<CertSpec>(common_installer::PrivilegeLevel::UNTRUSTED);
  std::shared_ptr<CertSpec> certPublic =
      std::make_shared<CertSpec>(common_installer::PrivilegeLevel::PUBLIC);
  std::shared_ptr<CertSpec> certParter =
      std::make_shared<CertSpec>(common_installer::PrivilegeLevel::PARTNER);
  std::shared_ptr<CertSpec> certPlatform =
      std::make_shared<CertSpec>(common_installer::PrivilegeLevel::PLATFORM);
  std::shared_ptr<BackgroundSuptSpec> background_supt =
      std::make_shared<BackgroundSuptSpec>();
  std::shared_ptr<KnownValueSpec> known =
      std::make_shared<KnownValueSpec>(known_background_categories);
  std::shared_ptr<UnknownValueSpec> unknown =
      std::make_shared<UnknownValueSpec>(not_unknown_background_categories);
  std::shared_ptr<SystemValueSpec> system = std::make_shared<SystemValueSpec>();
  std::shared_ptr<EmptyValueSpec> empty = std::make_shared<EmptyValueSpec>();

  std::shared_ptr<common_installer::ISpecification<BackgroundCatSpecTest>>
  certTrusted = certParter->Or(certPlatform);

  std::shared_ptr<common_installer::ISpecification<BackgroundCatSpecTest>>
  sentAllSpec = ver24->Not()->And(
      background_supt->And(certPublic->And(system->Not())->Or(certTrusted)));
  std::shared_ptr<common_installer::ISpecification<BackgroundCatSpecTest>>
  failSpec = certPublic->And(system)->Or(certUntrusted);
  std::shared_ptr<common_installer::ISpecification<BackgroundCatSpecTest>>
  systemSpec = certTrusted->And(system);
  std::shared_ptr<common_installer::ISpecification<BackgroundCatSpecTest>>
  knownSpec = failSpec->Not()->And(sentAllSpec->Not())->And(
      empty->Not())->And(known);
  std::shared_ptr<common_installer::ISpecification<BackgroundCatSpecTest>>
  unknownSpec = failSpec->Not()->And(empty->Not())->And(unknown);

  std::string str_ver(context_->manifest_data.get()->api_version);
  common_installer::PrivilegeLevel privilege_level =
      context_->privilege_level.get();
  const wgt::parse::SettingInfo& settings = static_cast<WgtBackendData*>(
      context_->backend_data.get())->settings.get();
  bool bkgnd_supt = settings.background_support_enabled();

  for (application_x* app :
      GListRange<application_x*>(context_->manifest_data.get()->application)) {
    std::set<std::string> bkgnd_cat;

    for (const char* background_category : GListRange<char*>(
        app->background_category)) {
      bkgnd_cat.insert(strdup(background_category));
    }

    g_list_free_full(app->background_category, g_free);
    app->background_category = nullptr;

    BackgroundCatSpecTest test =
        { str_ver, privilege_level, bkgnd_supt, std::move(bkgnd_cat) };

    if (failSpec->IsSatisfiedBy(test)) {
      LOG(DEBUG) << "Installation fail caused by background-category";
      return Status::ERROR;
    }

    if (sentAllSpec->IsSatisfiedBy(test)) {
      for (auto& background_category : known_background_categories) {
        app->background_category = g_list_append(
            app->background_category, strdup(background_category.c_str()));
      }
    }

    if (systemSpec->IsSatisfiedBy(test)) {
      app->background_category = g_list_append(
          app->background_category, strdup("system"));
    }

    if (knownSpec->IsSatisfiedBy(test)) {
      std::set<std::string> to_insert;

      std::set_intersection(
          known_background_categories.begin(),
          known_background_categories.end(),
          test.values.begin(), test.values.end(),
          std::inserter(to_insert, to_insert.begin()));

      for (auto &str : to_insert) {
        app->background_category = g_list_append(
            app->background_category, strdup(str.c_str()));
      }
    }

    if (unknownSpec->IsSatisfiedBy(test)) {
      std::set<std::string> to_insert;

      std::set_difference(
          test.values.begin(), test.values.end(),
          not_unknown_background_categories.begin(),
          not_unknown_background_categories.end(),
          std::inserter(to_insert, to_insert.end()));

      for (auto &str : to_insert) {
        app->background_category = g_list_append(
            app->background_category, strdup(str.c_str()));
      }
    }
  }

  return Status::OK;
}

}  // namespace security
}  // namespace wgt
