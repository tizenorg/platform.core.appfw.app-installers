// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "parser/manifest_handlers/service_handler.h"

#include <string.h>

#include <cassert>
#include <map>
#include <utility>
#include <set>

#include "utils/values.h"
#include "parser/application_manifest_constants.h"
#include "parser/manifest_util.h"

namespace ci = common_installer;
namespace keys = ci::application_widget_keys;

namespace {

std::unique_ptr<ci::parser::ServiceInfo> ParseServiceInfo(
    const ci::utils::Value& service_value) {
  const ci::utils::DictionaryValue* dict = nullptr;
  if (!service_value.GetAsDictionary(&dict)) {
    return nullptr;
  }
  std::string id;
  if (!dict->GetString(keys::kTizenServiceIdKey, &id))
    return false;
  std::unique_ptr<ci::parser::ServiceInfo> service(
      new ci::parser::ServiceInfo(id));

  std::string auto_restart = "false";
  if (dict->GetString(keys::kTizenServiceAutoRestartKey, &auto_restart))
    service->set_auto_restart(auto_restart == "true");

  std::string on_boot = "false";
  if (dict->GetString(keys::kTizenServiceOnBootKey, &on_boot))
    service->set_on_boot(on_boot == "true");

  return service;
}

}  // namespace

namespace common_installer {

namespace parser {

ServiceInfo::ServiceInfo(const std::string& id, bool auto_restart, bool on_boot)
    : id_(id), auto_restart_(auto_restart), on_boot_(on_boot) {
}

ServiceInfo::~ServiceInfo() {}

ServiceHandler::ServiceHandler() {}

ServiceHandler::~ServiceHandler() {}

bool ServiceHandler::Parse(std::shared_ptr<ApplicationData> application,
                          std::string* /*error*/) {
  const Manifest* manifest = application->GetManifest();
  assert(manifest);

  const utils::Value* services = nullptr;
  if (!manifest->Get(keys::kTizenServiceKey, &services)) {
    return true;
  }

  std::shared_ptr<ServiceList> services_data(new ServiceList());

  if (services->IsType(utils::Value::TYPE_DICTIONARY)) {
    auto service = ParseServiceInfo(*services);
    if (!service)
      return false;
    services_data->services.push_back(*service);
  } else if (services->IsType(utils::Value::TYPE_LIST)) {
    const utils::ListValue* list;
    services->GetAsList(&list);
    for (utils::ListValue::const_iterator it = list->begin();
         it != list->end(); ++it) {
      auto service = ParseServiceInfo(**it);
      if (!service)
        return false;
      services_data->services.push_back(*service);
    }
  }

  application->SetManifestData(keys::kTizenServiceKey, services_data);
  return true;
}

bool ServiceHandler::Validate(
    std::shared_ptr<const ApplicationData> application,
    std::string* error) const {

  const ServiceList* services =
      static_cast<ServiceList*>(
          application->GetManifestData(keys::kTizenServiceKey));

  if (!services)
    return true;

  for (auto& service : services->services) {
    if (!ValidateTizenApplicationId(service.id())) {
      *error = "The id property of application element "
               "does not match the format\n";
      return false;
    }
  }

  return true;
}

std::vector<std::string> ServiceHandler::Keys() const {
  return std::vector<std::string>(1, keys::kTizenServiceKey);
}

}  // namespace parser
}  // namespace common_installer
