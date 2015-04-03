/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "wgt/step/step_parse.h"

#include <string.h>

#include <pkgmgr/pkgmgr_parser.h>

#include <cstdio>
#include <cstdlib>
#include <set>
#include <string>
#include <vector>

#include "common/app_installer.h"
#include "common/context_installer.h"
#include "common/step/step.h"

#include "parser/manifest_handler.h"
#include "parser/manifest_constants.h"
#include "manifest_handlers/application_manifest_constants.h"
#include "manifest_handlers/appwidget_handler.h"
#include "manifest_handlers/category_handler.h"
#include "manifest_handlers/ime_handler.h"
#include "manifest_handlers/metadata_handler.h"
#include "manifest_handlers/navigation_handler.h"
#include "manifest_handlers/setting_handler.h"
#include "manifest_handlers/splash_screen_handler.h"


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
  return "1.0.0";
}

bool StepParse::FillIconPaths(manifest_x* manifest) {
  std::shared_ptr<const ApplicationIconsInfo> icons_info =
      std::static_pointer_cast<const ApplicationIconsInfo>(
          parser_->GetManifestData(manifest_keys::kIconsKey));
  if (icons_info.get()) {
    std::vector<std::string> icons = icons_info->get_icon_paths();
    for (std::string icon_str : icons) {
      icon_x* icon = reinterpret_cast<icon_x*> (calloc(1, sizeof(icon_x)));
      icon->name = strdup(icon_str.c_str());
      LISTADD(manifest->icon, icon);
    }
  }
  return true;
}

bool StepParse::FillWidgetInfo(manifest_x* manifest) {
  std::string short_name, version;
  std::shared_ptr<const WidgetInfo> wgt_info =
      std::static_pointer_cast<const WidgetInfo>(parser_->GetManifestData(
          parser::kWidgetKey));
  if (!wgt_info.get()) {
    LOG(ERROR) << "Widget info manifest data has not been found.";
    return false;
  }
  wgt_info->GetWidgetInfo()->GetString(app_keys::kShortNameKey, &short_name);
  wgt_info->GetWidgetInfo()->GetString(app_keys::kVersionKey, &version);

  manifest->version = strdup(GetPackageVersion(version).c_str());
  description_x* description = reinterpret_cast<description_x*>
      (calloc(1, sizeof(description_x)));
  std::string name;
  wgt_info->GetWidgetInfo()->GetString(manifest_keys::kNameKey, &name);
  description->name = strdup(name.c_str());
  manifest->description = description;

  manifest->label->name = strdup(name.c_str());

  // For wgt package use the long name if the short name is empty
  if (!short_name.empty())
    manifest->uiapplication->label->name = strdup(short_name.c_str());
  else
    manifest->uiapplication->label->name =
        strdup(manifest->description->name);
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
  api_version = app_info->required_version();
  manifest->uiapplication->appid = strdup(app_info->id().c_str());
  manifest->uiapplication->type = strdup("webapp");
  manifest->package = strdup(app_info->package().c_str());
  manifest->mainapp_id = strdup(app_info->id().c_str());
  manifest->type = strdup("wgt");

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
      app_control->operation =
          static_cast<operation_x*>(calloc(sizeof(operation_x), 1));
      app_control->operation->name = strdup(control.operation().c_str());
      app_control->mime =
          static_cast<mime_x*>(calloc(sizeof(mime_x), 1));
      app_control->mime->name = strdup(control.mime().c_str());
      app_control->uri =
          static_cast<uri_x*>(calloc(sizeof(uri_x), 1));
      app_control->uri->name = strdup(control.uri().c_str());
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
  // application data
  manifest->serviceapplication = nullptr;
  manifest->uiapplication = reinterpret_cast<uiapplication_x*>
    (calloc (1, sizeof(uiapplication_x)));
  manifest->uiapplication->label = reinterpret_cast<label_x*>
    (calloc(1, sizeof(label_x)));
  manifest->uiapplication->appcontrol = nullptr;
  manifest->label = reinterpret_cast<label_x*>(calloc(1, sizeof(label_x)));
  manifest->uiapplication->icon =
      reinterpret_cast<icon_x*> (calloc(1, sizeof(icon_x)));

  if (!FillWidgetInfo(manifest))
    return false;
  if (!FillApplicationInfo(manifest))
    return false;
  if (!FillIconPaths(manifest))
    return false;
  if (!FillPrivileges(manifest))
    return false;
  if (!FillAppControl(manifest))
    return false;

  if (manifest->icon)
    manifest->uiapplication->icon->name = strdup(manifest->icon->name);
  manifest->uiapplication->next = nullptr;
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

  std::unique_ptr<parser::ManifestHandlerRegistry> registry;
  registry.reset(new parser::ManifestHandlerRegistry(handlers));

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

  std::string name, short_name, version, required_api_version;
  wgt_info->GetWidgetInfo()->GetString(
      manifest_keys::kNameKey, &name);
  wgt_info->GetWidgetInfo()->GetString(
      app_keys::kShortNameKey, &short_name);
  wgt_info->GetWidgetInfo()->GetString(
      app_keys::kVersionKey, &version);
  required_api_version = info->required_version();

  context_->config_data.get().application_name.set(
      std::string(manifest->uiapplication->label->name));
  context_->config_data.get().required_version.set(required_api_version);
  context_->pkgid.set(std::string(manifest->package));

  std::shared_ptr<const PermissionsInfo> perm_info =
      std::static_pointer_cast<const PermissionsInfo>(
          parser_->GetManifestData(
              wgt::application_widget_keys::kTizenPermissionsKey));
  parser::PermissionSet permissions;
  if (perm_info)
     permissions = perm_info->GetAPIPermissions();

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
