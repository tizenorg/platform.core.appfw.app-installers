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

const std::vector<std::string> known_background_categories = {
    "media", "download", "background-network", "location",
    "sensor", "iot-communication" };
const std::vector<std::string> not_unknown_background_categories = {
    "media", "download", "background-network", "location",
    "sensor", "iot-communication", "system" };

struct BkgndCatSpecTest {
  unsigned int version;
  common_installer::PrivilegeLevel cert;
  bool background_supt;
  std::set<std::string> values;
};

class VersionAtLeastSpec :
    public common_installer::CompositeSpecification<BkgndCatSpecTest> {
  const unsigned int _version;

 public:
  explicit VersionAtLeastSpec(const unsigned int version) : _version(version) {}

  bool IsSatisfiedBy(const BkgndCatSpecTest &candidate) override {
    return candidate.version >= _version;
  }
};

class CertSpec :
    public common_installer::CompositeSpecification<BkgndCatSpecTest> {
  const common_installer::PrivilegeLevel _privilege;

 public:
  explicit CertSpec(const common_installer::PrivilegeLevel privilege) :
    _privilege(privilege) {}

  bool IsSatisfiedBy(const BkgndCatSpecTest &candidate) override {
    return candidate.cert == _privilege;
  }
};

class BackgroundSuptSpec :
    public common_installer::CompositeSpecification<BkgndCatSpecTest> {
 public:
  bool IsSatisfiedBy(const BkgndCatSpecTest &candidate) override {
    return candidate.background_supt == true;
  }
};

class KnownValueSpec :
    public common_installer::CompositeSpecification<BkgndCatSpecTest> {
  std::set<std::string> valid_values;

 public:
  explicit KnownValueSpec(const std::vector<std::string> values) {
    for (std::string str : values) {
      valid_values.insert(str);
    }
  }

  bool IsSatisfiedBy(const BkgndCatSpecTest &candidate) override {
    std::set<std::string> intersect;
    std::set_intersection(valid_values.begin(), valid_values.end(),
        candidate.values.begin(), candidate.values.end(),
        std::inserter(intersect, intersect.begin()));
    return !intersect.empty();
  }
};

class UnknownValueSpec :
    public common_installer::CompositeSpecification<BkgndCatSpecTest> {
  std::set<std::string> valid_values;

 public:
  explicit UnknownValueSpec(const std::vector<std::string> values) {
    for (std::string str : values) {
      valid_values.insert(str);
    }
  }

  bool IsSatisfiedBy(const BkgndCatSpecTest &candidate) override {
    std::set<std::string> diff;
    std::set_difference(candidate.values.begin(), candidate.values.end(),
        valid_values.begin(), valid_values.end(),
        std::inserter(diff, diff.begin()));
    return !diff.empty();
  }
};

class SystemValueSpec :
    public common_installer::CompositeSpecification<BkgndCatSpecTest> {
 public:
  bool IsSatisfiedBy(const BkgndCatSpecTest &candidate) override {
    return candidate.values.find("system") != candidate.values.end();
  }
};

class EmptyValueSpec :
    public common_installer::CompositeSpecification<BkgndCatSpecTest> {
 public:
  bool IsSatisfiedBy(const BkgndCatSpecTest &candidate) override {
    return candidate.values.empty();
  }
};

}  // namespace

namespace wgt {
namespace security {

common_installer::Step::Status StepCheckBackgroundCategory::process() {
  std::string str_ver(context_->manifest_data.get()->api_version);

  std::shared_ptr<VersionAtLeastSpec> ver23 =
      std::make_shared<VersionAtLeastSpec>(24);
  std::shared_ptr<CertSpec> certPublic =
      std::make_shared<CertSpec>(common_installer::PrivilegeLevel::PUBLIC);
  std::shared_ptr<CertSpec> certParter =
      std::make_shared<CertSpec>(common_installer::PrivilegeLevel::PARTNER);
  std::shared_ptr<BackgroundSuptSpec> background_supt =
      std::make_shared<BackgroundSuptSpec>();
  std::shared_ptr<KnownValueSpec> known =
      std::make_shared<KnownValueSpec>(known_background_categories);
  std::shared_ptr<UnknownValueSpec> unknown =
      std::make_shared<UnknownValueSpec>(not_unknown_background_categories);
  std::shared_ptr<SystemValueSpec> system = std::make_shared<SystemValueSpec>();
  std::shared_ptr<EmptyValueSpec> empty = std::make_shared<EmptyValueSpec>();

  std::shared_ptr<common_installer::ISpecification<BkgndCatSpecTest> >
  sentAllSpec = ver23->Not()->And(
      background_supt->And(certPublic->And(system->Not())->Or(certParter)));
  std::shared_ptr<common_installer::ISpecification<BkgndCatSpecTest> >
  failSpec = certPublic->And(system);
  std::shared_ptr<common_installer::ISpecification<BkgndCatSpecTest> >
  systemSpec = certParter->And(system);
  std::shared_ptr<common_installer::ISpecification<BkgndCatSpecTest> >
  knownSpec = failSpec->Not()->And(sentAllSpec->Not())->And(
      empty->Not())->And(known);
  std::shared_ptr<common_installer::ISpecification<BkgndCatSpecTest> >
  unknownSpec = failSpec->Not()->And(empty->Not())->And(unknown);

  unsigned int req_ver = common_installer::GetComparableVersion(str_ver);
  common_installer::PrivilegeLevel privil_lvl =
      context_->privilege_level.get();
  const wgt::parse::SettingInfo& settings = static_cast<WgtBackendData*>(
      context_->backend_data.get())->settings.get();
  bool bkgnd_supt = settings.background_support_enabled();

  for (application_x* app :
      GListRange<application_x*>(context_->manifest_data.get()->application)) {
    std::set<std::string> bkgnd_cat;

    for (const char* background_category : GListRange<char*>(
        app->background_category)) {
      bkgnd_cat.insert(background_category);
    }

    //FIXME(l.wysocki): passing bkgnd_cat by value;
    BkgndCatSpecTest test = { req_ver, privil_lvl, bkgnd_supt, bkgnd_cat };

    if (failSpec->IsSatisfiedBy(test)) {
      return Status::ERROR;
    }

    if (sentAllSpec->IsSatisfiedBy(test)) {
      //TODO(l.wysocki): send all
    }

    if (systemSpec->IsSatisfiedBy(test)) {
      //TODO(l.wysocki): send system
    }

    if (knownSpec->IsSatisfiedBy(test)) {
      //TODO(l.wysocki): send known
    }

    if (unknownSpec->IsSatisfiedBy(test)) {
      //TODO(l.wysocki): send unknown
    }

    //TODO(l.wysocki): update app->background_category
  }

  return Status::OK;
}

}  // namespace security
}  // namespace wgt
