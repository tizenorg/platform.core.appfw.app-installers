/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_parse.h"

#include <manifest_handlers/application_manifest_constants.h>
#include <manifest_handlers/appwidget_handler.h>
#include <manifest_handlers/category_handler.h>
#include <manifest_handlers/ime_handler.h>
#include <manifest_handlers/metadata_handler.h>
#include <manifest_handlers/navigation_handler.h>
#include <manifest_handlers/setting_handler.h>
#include <manifest_handlers/splash_screen_handler.h>
#include <manifest_parser/manifest_handler.h>
#include <manifest_parser/manifest_constants.h>

#include <pkgmgr/pkgmgr_parser.h>

#include <string.h>

#include <cstdio>
#include <cstdlib>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "common/app_installer.h"
#include "common/context_installer.h"
#include "common/step/step.h"

#include "wgt/wgt_backend_data.h"

namespace {

const std::string kManifestVersion = "1.0.0";

}

namespace wgt {
namespace parse {

namespace app_keys = wgt::application_widget_keys;
namespace manifest_keys = wgt::application_manifest_keys;

std::set<std::string> StepParse::ExtractPrivileges(
    std::shared_ptr<const PermissionsInfo> perm_info) const {
  return perm_info->GetAPIPermissions();
}

const std::string& StepParse::GetPackageVersion(
     const std::string& manifest_version) {
  if (!manifest_version.empty())
    return manifest_version;
  return kManifestVersion;
}

bool StepParse::FillIconPaths(manifest_x* manifest) {
  std::shared_ptr<const ApplicationIconsInfo> icons_info =
      std::static_pointer_cast<const ApplicationIconsInfo>(
          parser_->GetManifestData(manifest_keys::kIconsKey));
  if (icons_info.get()) {
    std::vector<std::string> icons = icons_info->get_icon_paths();
    for (auto& icon_str : icons) {
      icon_x* icon = reinterpret_cast<icon_x*> (calloc(1, sizeof(icon_x)));
      icon->name = strdup(icon_str.c_str());
      LISTADD(manifest->icon, icon);
    }
  }
  return true;
}

bool StepParse::FillWidgetInfo(manifest_x* manifest) {
  std::shared_ptr<const WidgetInfo> wgt_info =
      std::static_pointer_cast<const WidgetInfo>(parser_->GetManifestData(
          parser::kWidgetKey));
  if (!wgt_info.get()) {
    LOG(ERROR) << "Widget info manifest data has not been found.";
    return false;
  }

  const std::string& version = wgt_info->version();

  manifest->version = strdup(GetPackageVersion(version).c_str());

  for (auto& item : wgt_info->description_set()) {
    description_x* description = reinterpret_cast<description_x*>
        (calloc(1, sizeof(description_x)));
    description->name = strdup(item.second.c_str());
    description->lang = strdup(item.first.c_str());
    LISTADD(manifest->description, description);
  }

  for (auto& item : wgt_info->name_set()) {
    label_x* label = reinterpret_cast<label_x*>(calloc(1, sizeof(label_x)));
    label->name = strdup(item.second.c_str());
    label->lang = strdup(item.first.c_str());
    LISTADD(manifest->label, label);
  }

  manifest->type = strdup("wgt");

  // For wgt package use the long name if the short name is empty
  const auto& shorts = wgt_info->short_name_set();
  for (auto& item : wgt_info->name_set()) {
    label_x* label = reinterpret_cast<label_x*>(calloc(1, sizeof(label_x)));
    auto short_item = shorts.find(item.first);
    if (short_item != shorts.end()) {
      label->name = strdup(short_item->second.c_str());
      label->lang = strdup(short_item->first.c_str());
    } else {
      label->name = strdup(item.second.c_str());
      label->lang = strdup(item.first.c_str());
    }
    LISTADD(manifest->uiapplication->label, label);
  }

  return true;
}

bool StepParse::FillApplicationInfo(manifest_x* manifest) {
  std::string api_version;
  std::shared_ptr<const TizenApplicationInfo> app_info =
      std::static_pointer_cast<const TizenApplicationInfo>(
          parser_->GetManifestData(app_keys::kTizenApplicationKey));
  if (!app_info.get()) {
    LOG(ERROR) << "Application info manifest data has not been found.";
    return false;
  }
  // application data
  manifest->serviceapplication = nullptr;
  manifest->uiapplication = reinterpret_cast<uiapplication_x*>
    (calloc (1, sizeof(uiapplication_x)));
  manifest->uiapplication->appcontrol = nullptr;
  manifest->uiapplication->icon =
      reinterpret_cast<icon_x*> (calloc(1, sizeof(icon_x)));

  api_version = app_info->required_version();
  manifest->uiapplication->appid = strdup(app_info->id().c_str());
  manifest->uiapplication->type = strdup("webapp");

  if (manifest->icon)
    manifest->uiapplication->icon->name = strdup(manifest->icon->name);
  manifest->uiapplication->next = nullptr;

  manifest->package = strdup(app_info->package().c_str());
  manifest->mainapp_id = strdup(app_info->id().c_str());

  return true;
}

bool StepParse::FillAppControl(manifest_x* manifest) {
  std::shared_ptr<const AppControlInfoList> app_info_list =
      std::static_pointer_cast<const AppControlInfoList>(
          parser_->GetManifestData(app_keys::kTizenApplicationAppControlsKey));

  if (app_info_list) {
    for (const auto& control : app_info_list->controls) {
      appcontrol_x* app_control =
          static_cast<appcontrol_x*>(calloc(sizeof(appcontrol_x), 1));
      app_control->operation = strdup(control.operation().c_str());
      app_control->mime = strdup(control.mime().c_str());
      app_control->uri = strdup(control.uri().c_str());
      LISTADD(manifest->uiapplication->appcontrol, app_control);
    }
  }
  return true;
}

bool StepParse::FillPrivileges(manifest_x* manifest) {
  std::shared_ptr<const PermissionsInfo> perm_info =
      std::static_pointer_cast<const PermissionsInfo>(parser_->GetManifestData(
          app_keys::kTizenPermissionsKey));
  std::set<std::string> privileges;
  privileges.insert({"priv"});
  privileges.clear();
  if (perm_info)
    privileges = ExtractPrivileges(perm_info);

  if (!privileges.empty()) {
    privileges_x* privileges_x_list =
        reinterpret_cast<privileges_x*> (calloc(1, sizeof(privileges_x)));\
    manifest->privileges = privileges_x_list;
    for (const std::string& p : privileges) {
      privilege_x* privilege_x_node =
          reinterpret_cast<privilege_x*> (calloc(1, sizeof(privilege_x)));
      privilege_x_node->text = strdup(p.c_str());
      LISTADD(manifest->privileges->privilege, privilege_x_node);
    }
  }
  return true;
}

bool StepParse::FillManifestX(manifest_x* manifest) {
  if (!FillIconPaths(manifest))
    return false;
  if (!FillApplicationInfo(manifest))
    return false;
  if (!FillWidgetInfo(manifest))
    return false;
  if (!FillPrivileges(manifest))
    return false;
  if (!FillAppControl(manifest))
    return false;
  return true;
}

common_installer::Step::Status StepParse::process() {
  if (!StepParse::Check(context_->unpacked_dir_path.get())) {
    LOG(ERROR) << "No config.xml";
    return common_installer::Step::Status::ERROR;
  }

  std::vector<parser::ManifestHandler*> handlers = {
    new AppControlHandler,
    new ApplicationIconsHandler,
    new AppWidgetHandler,
    new CategoryHandler,
    new ImeHandler,
    new MetaDataHandler,
    new NavigationHandler,
    new PermissionsHandler,
    new SettingHandler,
    new SplashScreenHandler,
    new TizenApplicationHandler,
    new WidgetHandler
  };

  std::unique_ptr<parser::ManifestHandlerRegistry> registry(
      new parser::ManifestHandlerRegistry(handlers));

  parser_.reset(new parser::ManifestParser(std::move(registry)));
  if (!parser_->ParseManifest(config_)) {
    LOG(ERROR) << "[Parse] Parse failed. " <<  parser_->GetErrorMessage();
    return common_installer::Step::Status::ERROR;
  }

  const manifest_x* manifest = context_->manifest_data.get();
  if (!FillManifestX(const_cast<manifest_x*>(manifest))) {
    LOG(ERROR) << "[Parse] Storing manifest_x failed. "
               <<  parser_->GetErrorMessage();
    return common_installer::Step::Status::ERROR;
  }

  // Copy data from ManifestData to ContextInstaller
  std::shared_ptr<const TizenApplicationInfo> info =
      std::static_pointer_cast<const TizenApplicationInfo>(
          parser_->GetManifestData(
              wgt::application_widget_keys::kTizenApplicationKey));
  std::shared_ptr<const WidgetInfo> wgt_info =
      std::static_pointer_cast<const WidgetInfo>(
          parser_->GetManifestData(
              wgt::application_widget_keys::kTizenWidgetKey));

  std::string name;
  const auto& name_set = wgt_info->name_set();
  if (name_set.find("") != name_set.end())
    name = name_set.find("")->second;
  if (name_set.begin() != name_set.end())
    name = name_set.begin()->second;

  std::string short_name;
  const auto& short_name_set = wgt_info->short_name_set();
  if (short_name_set.find("") != short_name_set.end())
    short_name = short_name_set.find("")->second;
  if (short_name_set.begin() != short_name_set.end())
    short_name = short_name_set.begin()->second;

  const std::string& version = wgt_info->version();
  const std::string& required_api_version = info->required_version();

  if(manifest->uiapplication->label){
    context_->config_data.get().application_name.set(
        std::string(manifest->uiapplication->label->name));
  }

  context_->config_data.get().required_version.set(required_api_version);
  context_->pkgid.set(std::string(manifest->package));

  std::shared_ptr<const PermissionsInfo> perm_info =
      std::static_pointer_cast<const PermissionsInfo>(
          parser_->GetManifestData(
              wgt::application_widget_keys::kTizenPermissionsKey));
  parser::PermissionSet permissions;
  if (perm_info)
     permissions = perm_info->GetAPIPermissions();

  std::unique_ptr<WgtBackendData> backend_data(new WgtBackendData());

  std::shared_ptr<const SettingInfo> settings_info =
      std::static_pointer_cast<const SettingInfo>(
          parser_->GetManifestData(
              wgt::application_widget_keys::kTizenSettingKey));
  if (settings_info)
    backend_data->settings.set(*settings_info);

  context_->backend_data.set(backend_data.release());

  LOG(DEBUG) << " Read data -[ ";
  LOG(DEBUG) << "App id: " << info->id();
  LOG(DEBUG) << "  package     = " <<  info->package();
  LOG(DEBUG) << "  id          = " <<  info->id();
  LOG(DEBUG) << "  name        = " <<  name;
  LOG(DEBUG) << "  short_name  = " <<  short_name;
  LOG(DEBUG) << "  version     = " <<  version;
  LOG(DEBUG) << "  icon        = " <<  manifest->uiapplication->icon->name;
  LOG(DEBUG) << "  api_version = " <<  info->required_version();
  LOG(DEBUG) << "  privileges -[";
  for (const auto& p : permissions) {
    LOG(DEBUG) << "    " << p;
  }
  LOG(DEBUG) << "  ]-";
  LOG(DEBUG) << "]-";

  return common_installer::Step::Status::OK;
}

bool StepParse::Check(const boost::filesystem::path& widget_path) {
  boost::filesystem::path config = widget_path;
  config /= "config.xml";

  LOG(DEBUG) << "config.xml path: " << config;

  if (!boost::filesystem::exists(config))
    return false;

  config_ = config;
  return true;
}

}  // namespace parse
}  // namespace wgt