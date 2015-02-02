// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-xwalk file.

#include "widget-manifest-parser/manifest_handler.h"

#include <cassert>
#include <set>

#include "widget-manifest-parser/application_manifest_constants.h"

#include "widget-manifest-parser/manifest_handlers/permissions_handler.h"
#include "widget-manifest-parser/manifest_handlers/category_handler.h"
#include "widget-manifest-parser/manifest_handlers/ime_handler.h"
#include "widget-manifest-parser/manifest_handlers/setting_handler.h"
#include "widget-manifest-parser/manifest_handlers/tizen_application_handler.h"
#include "widget-manifest-parser/manifest_handlers/widget_handler.h"

// TODO(t.iwanek): add following handlers
// #include "widget-manifest-parser/manifest_handlers/tizen_appwidget_handler.h"
// #include "widget-manifest-parser/manifest_handlers/tizen_metadata_handler.h"
// #include "widget-manifest-parser/manifest_handlers/tizen_navigation_handler.h"
// #include "widget-manifest-parser/manifest_handlers/tizen_splash_screen_handler.h"

namespace common_installer {
namespace widget_manifest_parser {

namespace {

bool ValidateImeCategory(
    const common_installer::widget_manifest_parser::ApplicationData&
        application,
    std::string* error) {
  namespace keys = common_installer::application_widget_keys;
  // if config contains tizen:ime tag, proper category should be specified
  if (application.GetManifestData(keys::kTizenImeKey)) {
    const common_installer::widget_manifest_parser::CategoryInfoList*
        categories_list =
            static_cast<const CategoryInfoList*>(
                application.GetManifestData(keys::kTizenCategoryKey));

    if (categories_list) {
      const char imeCategory[] = "http://tizen.org/category/ime";
      for (const std::string& category : categories_list->categories) {
        if (category == imeCategory)
          return true;
      }
    }
    *error = "tizen:ime is specified but not proper category added";
    return false;
  }
  return true;
}

}  // namespace

ManifestHandler::~ManifestHandler() {
}

bool ManifestHandler::Validate(
    std::shared_ptr<const ApplicationData> application,
    std::string* error) const {
  return true;
}

bool ManifestHandler::AlwaysParseForType(Manifest::Type type) const {
  return false;
}

bool ManifestHandler::AlwaysValidateForType(Manifest::Type type) const {
  return false;
}

std::vector<std::string> ManifestHandler::PrerequisiteKeys() const {
  return std::vector<std::string>();
}

ManifestHandlerRegistry* ManifestHandlerRegistry::widget_registry_ = NULL;

ManifestHandlerRegistry::ManifestHandlerRegistry(
    const std::vector<ManifestHandler*>& handlers) {
  for (std::vector<ManifestHandler*>::const_iterator it = handlers.begin();
       it != handlers.end(); ++it) {
    Register(*it);
  }

  ReorderHandlersGivenDependencies();
}

ManifestHandlerRegistry::~ManifestHandlerRegistry() {
}

ManifestHandlerRegistry*
ManifestHandlerRegistry::GetInstance(Manifest::Type type) {
  return GetInstanceForWGT();
}

ManifestHandlerRegistry*
ManifestHandlerRegistry::GetInstanceForWGT() {
  if (widget_registry_)
    return widget_registry_;

  std::vector<ManifestHandler*> handlers;
  // We can put WGT specific manifest handlers here.
  handlers.push_back(new WidgetHandler);
  handlers.push_back(new TizenApplicationHandler);
  handlers.push_back(new PermissionsHandler);
  handlers.push_back(new CategoryHandler);
  handlers.push_back(new ImeHandler);
  handlers.push_back(new SettingHandler);

  widget_registry_ = new ManifestHandlerRegistry(handlers);
  return widget_registry_;
}

void ManifestHandlerRegistry::Register(ManifestHandler* handler) {
  const std::vector<std::string>& keys = handler->Keys();
  for (size_t i = 0; i < keys.size(); ++i) {
    handlers_[keys[i]] = handler;
  }
}

bool ManifestHandlerRegistry::ParseAppManifest(
    std::shared_ptr<ApplicationData> application, std::string* error) {
  std::map<int, ManifestHandler*> handlers_by_order;
  for (ManifestHandlerMap::iterator iter = handlers_.begin();
       iter != handlers_.end(); ++iter) {
    ManifestHandler* handler = iter->second;
    if (application->GetManifest()->HasPath(iter->first) ||
        handler->AlwaysParseForType(application->manifest_type())) {
      handlers_by_order[order_map_[handler]] = handler;
    }
  }
  for (std::map<int, ManifestHandler*>::iterator iter =
           handlers_by_order.begin();
       iter != handlers_by_order.end(); ++iter) {
    if (!(iter->second)->Parse(application, error)) {
      return false;
    }
  }
  return true;
}

bool ManifestHandlerRegistry::ValidateAppManifest(
    std::shared_ptr<const ApplicationData> application,
    std::string* error) {
  for (ManifestHandlerMap::iterator iter = handlers_.begin();
       iter != handlers_.end(); ++iter) {
    ManifestHandler* handler = iter->second;
    if ((application->GetManifest()->HasPath(iter->first) ||
         handler->AlwaysValidateForType(application->manifest_type())) &&
        !handler->Validate(application, error))
      return false;
  }

  if (!ValidateImeCategory(*application, error))
    return false;

  return true;
}

// static
void ManifestHandlerRegistry::SetInstanceForTesting(
    ManifestHandlerRegistry* registry, Manifest::Type type) {
  widget_registry_ = registry;
  return;
}

void ManifestHandlerRegistry::ReorderHandlersGivenDependencies() {
  std::set<ManifestHandler*> unsorted_handlers;
  for (ManifestHandlerMap::const_iterator iter = handlers_.begin();
       iter != handlers_.end(); ++iter) {
    unsorted_handlers.insert(iter->second);
  }

  int order = 0;
  while (true) {
    std::set<ManifestHandler*> next_unsorted_handlers;
    for (std::set<ManifestHandler*>::const_iterator iter =
             unsorted_handlers.begin();
         iter != unsorted_handlers.end(); ++iter) {
      ManifestHandler* handler = *iter;
      const std::vector<std::string>& prerequisites =
          handler->PrerequisiteKeys();
      int unsatisfied = prerequisites.size();
      for (size_t i = 0; i < prerequisites.size(); ++i) {
        ManifestHandlerMap::const_iterator prereq_iter =
            handlers_.find(prerequisites[i]);
        assert(prereq_iter != handlers_.end() &&
               "Application manifest handler depends on unrecognized key ");
        // Prerequisite is in our map.
        if (order_map_.find(prereq_iter->second) != order_map_.end())
          unsatisfied--;
      }
      if (unsatisfied == 0) {
        order_map_[handler] = order;
        order++;
      } else {
        // Put in the list for next time.
        next_unsorted_handlers.insert(handler);
      }
    }
    if (next_unsorted_handlers.size() == unsorted_handlers.size())
      break;
    unsorted_handlers.swap(next_unsorted_handlers);
  }

  // If there are any leftover unsorted handlers, they must have had
  // circular dependencies.
  assert(unsorted_handlers.empty() &&
         "Application manifest handlers have circular dependencies!");
}

}  // namespace widget_manifest_parser
}  // namespace common_installer
