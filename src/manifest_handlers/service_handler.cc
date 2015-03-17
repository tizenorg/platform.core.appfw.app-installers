// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "manifest_handlers/service_handler.h"

#include <cassert>
#include <map>
#include <utility>
#include <set>

#include "parser/manifest_util.h"
#include "parser/values.h"
#include "manifest_handlers/application_manifest_constants.h"

namespace keys = wgt::application_widget_keys;

namespace {

std::unique_ptr<wgt::parse::ServiceInfo> ParseServiceInfo(
    const parser::utils::Value& service_value) {
  const parser::utils::DictionaryValue* dict = nullptr;
  if (!service_value.GetAsDictionary(&dict)) {
    return nullptr;
  }
  std::string id;
  if (!dict->GetString(keys::kTizenServiceIdKey, &id))
    return false;
  std::unique_ptr<wgt::parse::ServiceInfo> service(
      new wgt::parse::ServiceInfo(id));

  std::string auto_restart = "false";
  if (dict->GetString(keys::kTizenServiceAutoRestartKey, &auto_restart))
    service->set_auto_restart(auto_restart == "true");

  std::string on_boot = "false";
  if (dict->GetString(keys::kTizenServiceOnBootKey, &on_boot))
    service->set_on_boot(on_boot == "true");

  return std::move(service);
}

}  // namespace

namespace wgt {

namespace parse {

ServiceInfo::ServiceInfo(const std::string& id, bool auto_restart, bool on_boot)
    : id_(id), auto_restart_(auto_restart), on_boot_(on_boot) {
}

ServiceInfo::~ServiceInfo() {}

ServiceHandler::ServiceHandler() {}

ServiceHandler::~ServiceHandler() {}

bool ServiceHandler::Parse(
    const parser::Manifest& manifest,
    std::shared_ptr<parser::ManifestData>* output,
    std::string* error) {
  const parser::utils::Value* services = nullptr;
  if (!manifest.Get(keys::kTizenServiceKey, &services)) {
    return true;
  }

  std::shared_ptr<ServiceList> services_data(new ServiceList());

  if (services->IsType(parser::utils::Value::TYPE_DICTIONARY)) {
    auto service = ParseServiceInfo(*services);
    if (!service) {
      *error = "Cannot parse tizen:service element";
      return false;
    }
    services_data->services.push_back(*service);
  } else if (services->IsType(parser::utils::Value::TYPE_LIST)) {
    const parser::utils::ListValue* list;
    services->GetAsList(&list);
    for (parser::utils::ListValue::const_iterator it = list->begin();
         it != list->end(); ++it) {
      auto service = ParseServiceInfo(**it);
      if (!service) {
        *error = "Cannot parse tizen:service element";
        return false;
      }
      services_data->services.push_back(*service);
    }
  }

  *output = std::static_pointer_cast<parser::ManifestData>(services_data);
  return true;
}

bool ServiceHandler::Validate(
    const parser::ManifestData& data,
    const parser::ManifestDataMap& /*handlers_output*/,
    std::string* error) const {
  const auto services = static_cast<const wgt::parse::ServiceList&>(data);

  for (auto& service : services.services) {
    if (!parser::ValidateTizenApplicationId(service.id())) {
      *error = "The id property of application element "
               "does not match the format\n";
      return false;
    }
  }

  return true;
}

std::string ServiceHandler::Key() const {
  return keys::kTizenServiceKey;
}

}  // namespace parse
}  // namespace wgt
