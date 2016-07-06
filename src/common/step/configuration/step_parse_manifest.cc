// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/configuration/step_parse_manifest.h"

#include <pkgmgr/pkgmgr_parser.h>
#include <pkgmgr-info.h>

#include <tpk_manifest_handlers/account_handler.h>
#include <tpk_manifest_handlers/application_manifest_constants.h>
#include <tpk_manifest_handlers/author_handler.h>
#include <tpk_manifest_handlers/description_handler.h>
#include <tpk_manifest_handlers/feature_handler.h>
#include <tpk_manifest_handlers/package_handler.h>
#include <tpk_manifest_handlers/privileges_handler.h>
#include <tpk_manifest_handlers/profile_handler.h>
#include <tpk_manifest_handlers/service_application_handler.h>
#include <tpk_manifest_handlers/shortcut_handler.h>
#include <tpk_manifest_handlers/ui_application_handler.h>
#include <tpk_manifest_handlers/watch_application_handler.h>
#include <tpk_manifest_handlers/widget_application_handler.h>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <set>
#include <type_traits>
#include <string>
#include <vector>

#include "common/app_installer.h"
#include "common/feature_validator.h"
#include "common/installer_context.h"
#include "common/paths.h"
#include "common/pkgmgr_registration.h"
#include "common/pkgmgr_query.h"
#include "common/step/step.h"
#include "common/utils/glist_range.h"

namespace app_keys = tpk::application_keys;
namespace bf = boost::filesystem;

namespace {

const char kManifestFileName[] = "tizen-manifest.xml";
const char kInstalledInternally[] = "installed_internal";
const utils::VersionNumber ver30("3.0");

}  // namepsace

namespace common_installer {
namespace configuration {

StepParseManifest::StepParseManifest(
    InstallerContext* context, ManifestLocation manifest_location,
    StoreLocation store_location)
    : Step(context),
      manifest_location_(manifest_location),
      store_location_(store_location) {
}

Step::Status StepParseManifest::precheck() {
  switch (manifest_location_) {
    case ManifestLocation::RECOVERY:
    case ManifestLocation::INSTALLED:
      if (context_->pkgid.get().empty()) {
        LOG(ERROR) << "Package id is not set";
        return Status::INVALID_VALUE;
      }
      break;
    case ManifestLocation::PACKAGE:
      if (context_->unpacked_dir_path.get().empty()) {
        LOG(ERROR) << "Unpacked directory doesn't exist";
        return Status::INVALID_VALUE;
      }
      break;
    default:
      LOG(ERROR) << "Unknown manifest location";
      return Status::INVALID_VALUE;
  }
  return Status::OK;
}

bool StepParseManifest::LocateConfigFile() {
  boost::filesystem::path manifest;
  switch (manifest_location_) {
    case ManifestLocation::RECOVERY: {
      context_->pkg_path.set(
          context_->root_application_path.get() / context_->pkgid.get());
      bf::path backup_path = common_installer::GetBackupPathForPackagePath(
          context_->pkg_path.get()) / kManifestFileName;
      bf::path in_package_path = context_->pkg_path.get() / kManifestFileName;
      bf::path install_path =
          bf::path(getUserManifestPath(context_->uid.get(), false))
              / bf::path(context_->pkgid.get());
      install_path += ".xml";
      if (bf::exists(backup_path))
        manifest = backup_path;
      else if (bf::exists(in_package_path))
        manifest = in_package_path;
      else if (bf::exists(install_path))
        manifest = install_path;
      break;
    }
    case ManifestLocation::INSTALLED: {
      bf::path xml_path =
          bf::path(getUserManifestPath(context_->uid.get(),
              context_->is_preload_request.get()))
          / bf::path(context_->pkgid.get());
      xml_path += ".xml";
      context_->xml_path.set(xml_path);
      manifest = context_->xml_path.get();
      break;
    }
    case ManifestLocation::PACKAGE: {
      manifest = context_->unpacked_dir_path.get();
      manifest /= kManifestFileName;
      break;
    }
    default: {
      LOG(ERROR) << "Unknown manifest location value";
      return false;
    }
  }

  LOG(DEBUG) << "manifest path: " << manifest;

  if (!boost::filesystem::exists(manifest))
    return false;

  path_ = manifest;
  return true;
}

bool StepParseManifest::FillInstallationInfo(manifest_x* manifest) {
  manifest->root_path = strdup(
      (context_->root_application_path.get() / manifest->package).c_str());
  manifest->installed_time =
      strdup(std::to_string(std::chrono::system_clock::to_time_t(
          std::chrono::system_clock::now())).c_str());
  return true;
}

bool StepParseManifest::FillPackageInfo(manifest_x* manifest) {
  std::shared_ptr<const tpk::parse::PackageInfo> pkg_info =
      std::static_pointer_cast<const tpk::parse::PackageInfo>(
          parser_->GetManifestData(app_keys::kManifestKey));
  if (!pkg_info) {
    LOG(ERROR) << "Package info manifest data has not been found.";
    return false;
  }

  auto ui_application_list =
      std::static_pointer_cast<const tpk::parse::UIApplicationInfoList>(
          parser_->GetManifestData(app_keys::kUIApplicationKey));
  auto service_application_list =
      std::static_pointer_cast<const tpk::parse::ServiceApplicationInfoList>(
          parser_->GetManifestData(app_keys::kServiceApplicationKey));
  auto widget_application_list =
      std::static_pointer_cast<const tpk::parse::WidgetApplicationInfoList>(
          parser_->GetManifestData(app_keys::kWidgetApplicationKey));
  auto watch_application_list =
      std::static_pointer_cast<const tpk::parse::WatchApplicationInfoList>(
          parser_->GetManifestData(app_keys::kWatchApplicationKey));

  // mandatory check
  if (!ui_application_list && !service_application_list &&
      !widget_application_list && !watch_application_list) {
    LOG(ERROR) << "UI Application or Service Application or Widget Application "
                  "or Watch Application are mandatory and has not been found.";
    return false;
  }

  manifest->ns = strdup(pkg_info->xmlns().c_str());
  manifest->package = strdup(pkg_info->package().c_str());
  manifest->nodisplay_setting = strdup(pkg_info->nodisplay_setting().c_str());
  manifest->appsetting = strdup("false");
  manifest->support_disable = strdup("false");
  manifest->version = strdup(pkg_info->version().c_str());
  manifest->installlocation = strdup(pkg_info->install_location().c_str());
  manifest->api_version = strdup(pkg_info->api_version().c_str());
  manifest->preload = strdup(pkg_info->preload().c_str());

  if (pkg_info->type().empty()) {
    common_installer::RequestType req_type = context_->request_type.get();
    if (req_type == RequestType::ManifestDirectInstall ||
        req_type == RequestType::ManifestDirectUpdate) {
      manifest->type = strdup("rpm");
      if (!context_->is_preload_request.get()) {
        LOG(ERROR) << "Non-preload rpm installation not allowed";
        return false;
      }
    } else {
      manifest->type = strdup("tpk");
    }
  } else {
    manifest->type = strdup(pkg_info->type().c_str());
  }

  for (auto& pair : pkg_info->labels()) {
    label_x* label = reinterpret_cast<label_x*>(calloc(1, sizeof(label_x)));
    if (!pair.first.empty())
      label->lang = strdup(pair.first.c_str());
    else
      label->lang = strdup(DEFAULT_LOCALE);
    label->text = strdup(pair.second.c_str());
    manifest->label = g_list_append(manifest->label, label);
  }

  std::shared_ptr<const tpk::parse::ProfileInfo> profile_info =
      std::static_pointer_cast<const tpk::parse::ProfileInfo>(
          parser_->GetManifestData(tpk::parse::ProfileInfo::Key()));
  if (profile_info) {
    for (auto& profile : profile_info->profiles()) {
      manifest->deviceprofile = g_list_append(manifest->deviceprofile,
                                              strdup(profile.c_str()));
    }
  }

  // set installed_storage if package is installed
  // this is internal field in package manager but after reading configuration
  // we must know it
  if (manifest_location_ == ManifestLocation::INSTALLED ||
      manifest_location_ == ManifestLocation::RECOVERY) {
    std::string storage = QueryStorageForPkgId(manifest->package,
                                              context_->uid.get());
    if (storage.empty()) {
        // Failed to query installation storage, assign internal for preloaded
        // applications
        manifest->installed_storage = strdup(kInstalledInternally);
    } else {
        manifest->installed_storage = strdup(storage.c_str());
    }
  } else {
    manifest->installed_storage = strdup(kInstalledInternally);
  }

  if (ui_application_list) {
    manifest->mainapp_id =
        strdup(ui_application_list->items[0].app_info.appid().c_str());
  } else if (service_application_list) {
    manifest->mainapp_id =
        strdup(service_application_list->items[0].app_info.appid().c_str());
  } else if (widget_application_list) {
    manifest->mainapp_id =
        strdup(widget_application_list->items[0].app_info.appid().c_str());
  } else if (watch_application_list) {
    manifest->mainapp_id =
        strdup(watch_application_list->items[0].app_info.appid().c_str());
  }
  return true;
}

bool StepParseManifest::FillAuthorInfo(manifest_x* manifest) {
  std::shared_ptr<const tpk::parse::AuthorInfo> author_info =
      std::static_pointer_cast<const tpk::parse::AuthorInfo>(
          parser_->GetManifestData(tpk::parse::AuthorInfo::Key()));

  if (!author_info)
    return true;

  author_x* author = reinterpret_cast<author_x*>(calloc(1, sizeof(author_x)));
  author->text = strdup(author_info->name().c_str());
  author->email = strdup(author_info->email().c_str());
  author->href = strdup(author_info->href().c_str());
  author->lang = strdup(DEFAULT_LOCALE);
  manifest->author = g_list_append(manifest->author, author);
  return true;
}

bool StepParseManifest::FillDescriptionInfo(manifest_x* manifest) {
  std::shared_ptr<const tpk::parse::DescriptionInfoList> description_info =
      std::static_pointer_cast<const tpk::parse::DescriptionInfoList>(
          parser_->GetManifestData(tpk::parse::DescriptionInfoList::Key()));

  if (!description_info)
    return true;

  for (auto& desc : description_info->descriptions) {
    description_x* description = reinterpret_cast<description_x*>
        (calloc(1, sizeof(description_x)));
    description->text = strdup(desc.description().c_str());
    description->lang = !desc.xml_lang().empty() ?
        strdup(desc.xml_lang().c_str()) : strdup(DEFAULT_LOCALE);
    manifest->description = g_list_append(manifest->description, description);
  }
  return true;
}

bool StepParseManifest::FillPrivileges(manifest_x* manifest) {
  std::shared_ptr<const tpk::parse::PrivilegesInfo> perm_info =
      std::static_pointer_cast<const tpk::parse::PrivilegesInfo>(
          parser_->GetManifestData(app_keys::kPrivilegesKey));
  if (!perm_info)
    return true;

  std::set<std::string> privileges = perm_info->GetPrivileges();
  for (auto& priv : privileges) {
    manifest->privileges = g_list_append(manifest->privileges,
                                         strdup(priv.c_str()));
  }
  return true;
}

bool StepParseManifest::FillWidgetApplication(manifest_x* manifest) {
  auto widget_application_list =
      std::static_pointer_cast<const tpk::parse::WidgetApplicationInfoList>(
          parser_->GetManifestData(app_keys::kWidgetApplicationKey));
  if (!widget_application_list)
    return true;

  for (const auto& application : widget_application_list->items) {
    // if there is no app yet, set this app as mainapp
    bool main_app = manifest->application == nullptr;

    application_x* widget_app =
        static_cast<application_x*>(calloc(1, sizeof(application_x)));
    widget_app->appid = strdup(application.app_info.appid().c_str());
    widget_app->launch_mode =
        strdup(application.app_info.launch_mode().c_str());
    widget_app->multiple = strdup("false");
    widget_app->nodisplay = strdup("true");
    widget_app->taskmanage = strdup("false");
    widget_app->indicatordisplay = strdup("false");
    widget_app->type = strdup("capp");
    widget_app->component_type = strdup("widgetapp");
    widget_app->hwacceleration =
        strdup(application.app_info.hwacceleration().c_str());
    widget_app->onboot = strdup("false");
    widget_app->autorestart = strdup("false");
    widget_app->mainapp = main_app ? strdup("true") : strdup("false");
    widget_app->enabled = strdup("true");
    widget_app->screenreader = strdup("use-system-setting");
    widget_app->recentimage = strdup("false");
    widget_app->launchcondition = strdup("false");
    widget_app->guestmode_visibility = strdup("true");
    widget_app->permission_type = strdup("normal");
    widget_app->ambient_support = strdup("false");
    widget_app->effectimage_type = strdup("image");
    widget_app->submode = strdup("false");
    widget_app->process_pool = strdup("false");
    widget_app->package = strdup(manifest->package);
    widget_app->support_disable = strdup(manifest->support_disable);
    manifest->application = g_list_append(manifest->application, widget_app);
    if (bf::path(application.app_info.exec().c_str()).is_absolute())
      widget_app->exec = strdup(application.app_info.exec().c_str());
    else
      widget_app->exec = strdup((context_->root_application_path.get()
                            / manifest->package / "bin"
                            / application.app_info.exec()).c_str());

    if (!FillApplicationIconPaths(widget_app, application.app_icons))
      return false;
    if (!FillLabel(widget_app, application.label))
      return false;
    if (!FillImage(widget_app, application.app_images))
      return false;
    if (!FillCategories(widget_app, application.categories))
      return false;
    if (!FillMetadata(widget_app, application.meta_data))
      return false;
  }
  return true;
}

bool StepParseManifest::FillServiceApplication(manifest_x* manifest) {
  auto service_application_list =
      std::static_pointer_cast<const tpk::parse::ServiceApplicationInfoList>(
          parser_->GetManifestData(app_keys::kServiceApplicationKey));
  if (!service_application_list)
    return true;

  for (const auto& application : service_application_list->items) {
    // if there is no app yet, set this app as mainapp
    bool main_app = manifest->application == nullptr;

    application_x* service_app =
        static_cast<application_x*>(calloc(1, sizeof(application_x)));
    service_app->appid = strdup(application.app_info.appid().c_str());
    service_app->multiple = strdup(application.app_info.multiple().c_str());
    service_app->taskmanage = strdup(application.app_info.taskmanage().c_str());
    service_app->autorestart =
        strdup(application.app_info.auto_restart().c_str());
    service_app->onboot = strdup(application.app_info.on_boot().c_str());
    service_app->type = strdup(application.app_info.type().c_str());
    service_app->process_pool =
        strdup(application.app_info.process_pool().c_str());
    service_app->component_type = strdup("svcapp");
    service_app->mainapp = main_app ? strdup("true") : strdup("false");
    service_app->enabled = strdup("true");
    service_app->nodisplay = strdup("true");
    service_app->hwacceleration = strdup("default");
    service_app->screenreader = strdup("use-system-setting");
    service_app->recentimage = strdup("false");
    service_app->launchcondition = strdup("false");
    service_app->indicatordisplay = strdup("true");
    service_app->effectimage_type = strdup("image");
    service_app->guestmode_visibility = strdup("true");
    service_app->permission_type = strdup("normal");
    service_app->submode = strdup("false");
    service_app->process_pool = strdup("false");
    service_app->ambient_support = strdup("false");
    service_app->package = strdup(manifest->package);
    service_app->support_disable = strdup(manifest->support_disable);
    manifest->application = g_list_append(manifest->application, service_app);
    if (bf::path(application.app_info.exec().c_str()).is_absolute())
      service_app->exec = strdup(application.app_info.exec().c_str());
    else
      service_app->exec = strdup((context_->root_application_path.get()
                            / manifest->package / "bin"
                            / application.app_info.exec()).c_str());

    if (!FillAppControl(service_app,  application.app_control))
      return false;
    if (!FillDataControl(service_app, application.data_control))
      return false;
    if (!FillApplicationIconPaths(service_app, application.app_icons))
      return false;
    if (!FillLabel(service_app, application.label))
      return false;
    if (!FillMetadata(service_app, application.meta_data))
      return false;
    if (!FillCategories(service_app, application.categories))
      return false;
    if (!FillBackgroundCategoryInfo(service_app,
        application.background_category))
      return false;
  }
  return true;
}

bool StepParseManifest::FillUIApplication(manifest_x* manifest) {
  std::shared_ptr<const tpk::parse::UIApplicationInfoList> ui_application_list =
      std::static_pointer_cast<const tpk::parse::UIApplicationInfoList>(
          parser_->GetManifestData(app_keys::kUIApplicationKey));
  if (!ui_application_list)
    return true;

  for (const auto& application : ui_application_list->items) {
    // if there is no app yet, set this app as mainapp
    bool main_app = manifest->application == nullptr;

    application_x* ui_app =
        static_cast<application_x*>(calloc(1, sizeof(application_x)));
    ui_app->appid = strdup(application.app_info.appid().c_str());
    ui_app->launch_mode = strdup(application.app_info.launch_mode().c_str());
    ui_app->multiple = strdup(application.app_info.multiple().c_str());
    ui_app->nodisplay = strdup(application.app_info.nodisplay().c_str());
    ui_app->taskmanage = strdup(application.app_info.taskmanage().c_str());
    ui_app->type = strdup(application.app_info.type().c_str());
    ui_app->ui_gadget = strdup(application.app_info.uigadget().c_str());
    ui_app->process_pool = strdup(application.app_info.process_pool().c_str());
    ui_app->submode = strdup(application.app_info.submode().c_str());
    if (!application.app_info.indicator_display().empty())
      ui_app->indicatordisplay =
          strdup(application.app_info.indicator_display().c_str());
    if (!application.app_info.effectimage_type().empty())
      ui_app->effectimage_type =
          strdup(application.app_info.effectimage_type().c_str());
    if (!application.app_info.portrait_image().empty())
      ui_app->portraitimg =
          strdup(application.app_info.portrait_image().c_str());
    if (!application.app_info.landscape_image().empty())
      ui_app->landscapeimg =
          strdup(application.app_info.landscape_image().c_str());
    ui_app->submode_mainid =
        strdup(application.app_info.submode_mainid().c_str());
    ui_app->hwacceleration =
        strdup(application.app_info.hwacceleration().c_str());
    ui_app->onboot = strdup("false");
    ui_app->autorestart = strdup("false");
    ui_app->component_type = strdup("uiapp");
    ui_app->mainapp = main_app ? strdup("true") : strdup("false");
    ui_app->enabled = strdup("true");
    ui_app->screenreader = strdup("use-system-setting");
    ui_app->recentimage = strdup("false");
    ui_app->launchcondition = strdup("false");
    ui_app->guestmode_visibility = strdup("true");
    ui_app->permission_type = strdup("normal");
    ui_app->ambient_support = strdup("false");
    ui_app->package = strdup(manifest->package);
    ui_app->support_disable = strdup(manifest->support_disable);
    ui_app->splash_screen_display =
        strdup(application.app_info.splash_screen_display().c_str());
    manifest->application = g_list_append(manifest->application, ui_app);
    if (bf::path(application.app_info.exec().c_str()).is_absolute())
      ui_app->exec = strdup(application.app_info.exec().c_str());
    else
      ui_app->exec = strdup((context_->root_application_path.get()
                            / manifest->package / "bin"
                            / application.app_info.exec()).c_str());


    if (!FillAppControl(ui_app, application.app_control))
      return false;
    if (!FillDataControl(ui_app, application.data_control))
      return false;
    if (!FillApplicationIconPaths(ui_app, application.app_icons))
      return false;
    if (!FillLabel(ui_app, application.label))
      return false;
    if (!FillImage(ui_app, application.app_images))
      return false;
    if (!FillMetadata(ui_app, application.meta_data))
      return false;
    if (!FillCategories(ui_app, application.categories))
      return false;
    if (!FillBackgroundCategoryInfo(ui_app, application.background_category))
      return false;
    if (!FillSplashScreen(ui_app, application.app_splashscreens))
      return false;
  }
  return true;
}

bool StepParseManifest::FillWatchApplication(manifest_x* manifest) {
  auto watch_application_list =
        std::static_pointer_cast<const tpk::parse::WatchApplicationInfoList>(
            parser_->GetManifestData(app_keys::kWatchApplicationKey));
  if (!watch_application_list)
    return true;

  for (const auto& watch_application : watch_application_list->items) {
    bool main_app = manifest->application == nullptr;

    application_x* watch_app =
             static_cast<application_x*>(calloc(1, sizeof(application_x)));
    watch_app->appid = strdup(watch_application.app_info.appid().c_str());

    if (bf::path(watch_application.app_info.exec().c_str()).is_absolute())
      watch_app->exec = strdup(watch_application.app_info.exec().c_str());
    else
      watch_app->exec = strdup(
          (context_->root_application_path.get()
                               / manifest->package / "bin" /
                               watch_application.app_info.exec()).c_str());
    watch_app->nodisplay = strdup("true");
    watch_app->multiple = strdup("false");
    watch_app->type = strdup(watch_application.app_info.type().c_str());
    watch_app->taskmanage = strdup("false");
    watch_app->enabled = strdup("true");
    watch_app->hwacceleration = strdup("default");
    watch_app->screenreader = strdup("use-system-setting");
    watch_app->mainapp = main_app ? strdup("true") : strdup("false");
    watch_app->recentimage = strdup("false");
    watch_app->launchcondition = strdup("false");
    watch_app->indicatordisplay = strdup("true");
    watch_app->effectimage_type = strdup("image");
    watch_app->guestmode_visibility = strdup("true");
    watch_app->permission_type = strdup("normal");
    watch_app->component_type = strdup("watchapp");
    watch_app->preload = strdup("false");
    watch_app->submode = strdup("false");
    watch_app->process_pool = strdup("false");
    watch_app->autorestart = strdup("false");
    watch_app->onboot = strdup("false");
    watch_app->support_disable = strdup(manifest->support_disable);
    watch_app->ui_gadget = strdup("false");
    watch_app->launch_mode = strdup("single");
    watch_app->ambient_support =
        strdup(watch_application.app_info.ambient_support().c_str());
    watch_app->package = strdup(manifest->package);
    if (!FillLabel(watch_app, watch_application.label))
      return false;
    if (!FillApplicationIconPaths(watch_app, watch_application.app_icons))
      return false;
    if (!FillMetadata(watch_app, watch_application.meta_data))
      return false;
    if (!FillCategories(watch_app, watch_application.categories))
      return false;
    if (!FillBackgroundCategoryInfo(watch_app,
        watch_application.background_category))
      return false;
    manifest->application = g_list_append(manifest->application, watch_app);
  }
  return true;
}

bool StepParseManifest::CheckFeatures() {
  auto feature_info =
        std::static_pointer_cast<const tpk::parse::FeatureInfo>(
            parser_->GetManifestData(tpk::parse::FeatureInfo::Key()));
  if (!feature_info)
    return true;

  std::string error;
  FeatureValidator validator(feature_info->features());
  if (!validator.Validate(&error)) {
    LOG(ERROR) << "Feature validation error. " << error;
    return false;
  }

  return true;
}

template <typename T>
bool StepParseManifest::FillAppControl(application_x* app,
                                       const T& app_control_list) {
  if (app_control_list.empty())
    return true;

  for (const auto& control : app_control_list) {
    appcontrol_x* app_control =
          static_cast<appcontrol_x*>(calloc(1, sizeof(appcontrol_x)));
    app_control->operation = strdup(control.operation().c_str());
    if (!control.mime().empty())
      app_control->mime = strdup(control.mime().c_str());
    if (!control.uri().empty())
      app_control->uri = strdup(control.uri().c_str());
    app->appcontrol = g_list_append(app->appcontrol, app_control);
  }
  return true;
}

template <typename T>
bool StepParseManifest::FillDataControl(application_x* app,
                                const T& data_control_list) {
  if (data_control_list.empty())
    return true;

  for (const auto& control : data_control_list) {
    datacontrol_x* data_control =
          static_cast<datacontrol_x*>(calloc(1, sizeof(datacontrol_x)));
    data_control->access = strdup(control.access().c_str());
    data_control->providerid = strdup(control.providerid().c_str());
    data_control->type = strdup(control.type().c_str());
    app->datacontrol = g_list_append(app->datacontrol, data_control);
  }
  return true;
}

template <typename T>
bool StepParseManifest::FillApplicationIconPaths(application_x* app,
                                         const T& icons_info) {
  for (auto& application_icon : icons_info.icons()) {
    icon_x* icon = reinterpret_cast<icon_x*> (calloc(1, sizeof(icon_x)));
    bf::path text;
    if (bf::path(application_icon.path()).is_absolute()) {
      text = application_icon.path();
    } else {
      text = context_->root_application_path.get()
          / context_->pkgid.get() / "shared" / "res" / application_icon.path();
    }
    // NOTE: name is an attribute, but the xml writer uses it as text.
    // This must be fixed in whole app-installer modules, including wgt.
    // Current implementation is just for compatibility.
    icon->text = strdup(text.c_str());
    if (application_icon.lang().empty())
      icon->lang = strdup(DEFAULT_LOCALE);
    else
      icon->lang = strdup(application_icon.lang().c_str());

    if (!application_icon.dpi().empty())
      icon->dpi = strdup(application_icon.dpi().c_str());
    app->icon = g_list_append(app->icon, icon);
  }
  return true;
}

template <typename T>
bool StepParseManifest::FillLabel(application_x* app, const T& label_list) {
  if (label_list.empty())
    return true;

  for (const auto& control : label_list) {
    label_x* label =
          static_cast<label_x*>(calloc(1, sizeof(label_x)));
    // NOTE: name is an attribute, but the xml writer uses it as text.
    // This must be fixed in whole app-installer modules, including wgt.
    // Current implementation is just for compatibility.
    label->text = strdup(control.text().c_str());
    label->name = strdup(control.name().c_str());
    label->lang = !control.xml_lang().empty() ?
        strdup(control.xml_lang().c_str()) : strdup(DEFAULT_LOCALE);
    app->label = g_list_append(app->label, label);
  }
  return true;
}

template <typename T>
bool StepParseManifest::FillMetadata(application_x* app,
                                     const T& meta_data_list) {
  if (meta_data_list.empty())
    return true;

  for (auto& meta : meta_data_list) {
    metadata_x* metadata =
        static_cast<metadata_x*>(calloc(1, sizeof(metadata_x)));
    metadata->key = strdup(meta.key().c_str());
    metadata->value = strdup(meta.val().c_str());
    app->metadata = g_list_append(app->metadata, metadata);
  }
  return true;
}

template <typename T>
bool StepParseManifest::FillCategories(application_x* manifest,
                                     const T& categories) {
  for (auto& category : categories) {
    manifest->category = g_list_append(manifest->category,
                                       strdup(category.c_str()));
  }
  return true;
}

template <typename T>
bool StepParseManifest::FillSplashScreen(application_x* app,
                                     const T& splashscreens_info) {
  for (auto& splash_screen : splashscreens_info.splashscreens()) {
    splashscreen_x* splashscreen =
        static_cast<splashscreen_x*>(calloc(1, sizeof(splashscreen_x)));
    if (context_->is_preload_request.get())
      splashscreen->src = strdup(splash_screen.src().c_str());
    else
      splashscreen->src = strdup((context_->root_application_path.get()
        / app->package / "shared" / "res" / splash_screen.src()).c_str());

    splashscreen->type = strdup(splash_screen.type().c_str());
    if (!splash_screen.dpi().empty())
      splashscreen->dpi = strdup(splash_screen.dpi().c_str());
    splashscreen->orientation = strdup(splash_screen.orientation().c_str());
    if (!splash_screen.indicatordisplay().empty())
      splashscreen->indicatordisplay = strdup(
          splash_screen.indicatordisplay().c_str());
    else
      splashscreen->indicatordisplay = strdup("true");
    if (!splash_screen.operation().empty())
      splashscreen->operation = strdup(splash_screen.operation().c_str());
    if (!splash_screen.colordepth().empty())
      splashscreen->color_depth = strdup(splash_screen.colordepth().c_str());
    else
      splashscreen->color_depth = strdup("24");
    app->splashscreens = g_list_append(app->splashscreens, splashscreen);
  }
  return true;
}

bool StepParseManifest::FillImage(application_x* app,
                          const tpk::parse::ApplicationImagesInfo& image_list) {
  for (auto& app_image : image_list.images) {
    image_x* image =
        static_cast<image_x*>(calloc(1, sizeof(image_x)));
    const std::string& lang = app_image.lang();
    if (!lang.empty())
      image->lang = strdup(lang.c_str());
    else
      image->lang = strdup(DEFAULT_LOCALE);
    if (!app_image.section().empty())
      image->section = strdup(app_image.section().c_str());
    app->image = g_list_append(app->image, image);
  }
  return true;
}

template <typename T>
bool StepParseManifest::FillBackgroundCategoryInfo(application_x* app,
    const T& background_category_data_list) {
  for (const auto& background_category : background_category_data_list) {
    app->background_category = g_list_append(
        app->background_category, strdup(background_category.value().c_str()));
  }

  return true;
}

bool StepParseManifest::FillManifestX(manifest_x* manifest) {
  if (!FillPackageInfo(manifest))
    return false;
  if (!FillInstallationInfo(manifest))
    return false;
  if (!FillUIApplication(manifest))
    return false;
  if (!FillServiceApplication(manifest))
    return false;
  if (!FillWidgetApplication(manifest))
    return false;
  if (!FillWatchApplication(manifest))
    return false;
  if (!FillPrivileges(manifest))
    return false;
  if (!FillAuthorInfo(manifest))
    return false;
  if (!FillDescriptionInfo(manifest))
    return false;
  return true;
}

Step::Status StepParseManifest::process() {
  if (!LocateConfigFile()) {
    // continue if this is recovery, manifest file may never been created
    if (manifest_location_ == ManifestLocation::RECOVERY) {
      LOG(DEBUG) << "Manifest for recovery not found";
      return Step::Status::OK;
    }
    LOG(ERROR) << "No manifest file exists";
    return Step::Status::MANIFEST_NOT_FOUND;
  }
  parser_.reset(new tpk::parse::TPKConfigParser());
  if (!parser_->ParseManifest(path_)) {
    LOG(ERROR) << "[Parse] Parse failed. " <<  parser_->GetErrorMessage();
    return Step::Status::PARSE_ERROR;
  }

  // Copy data from ManifestData to InstallerContext
  std::shared_ptr<const tpk::parse::PackageInfo> info =
      std::static_pointer_cast<const tpk::parse::PackageInfo>(
          parser_->GetManifestData(app_keys::kManifestKey));

  context_->pkgid.set(info->package());
  context_->pkg_path.set(
      context_->root_application_path.get() / context_->pkgid.get());

  manifest_x* manifest =
      static_cast<manifest_x*>(calloc(1, sizeof(manifest_x)));

  if (!FillManifestX(const_cast<manifest_x*>(manifest))) {
    LOG(ERROR) << "[Parse] Storing manifest_x failed. "
               <<  parser_->GetErrorMessage();
    return Step::Status::PARSE_ERROR;
  }

  std::string str_ver(manifest->api_version);
  utils::VersionNumber api_version(str_ver);
  if (api_version >= ver30 &&
      context_->installation_mode.get() != InstallationMode::OFFLINE) {
    if (!CheckFeatures())
      return Status::PARSE_ERROR;
  }

  if (manifest_location_ == ManifestLocation::INSTALLED) {
    // recovery of tep value for installed package
    std::string old_tep =
        QueryTepPath(context_->pkgid.get(), context_->uid.get());
    if (!old_tep.empty())
      manifest->tep_name = strdup(old_tep.c_str());

    // recovery of zip mount file for installed package
    std::string zip_mount_file =
        QueryZipMountFile(context_->pkgid.get(), context_->uid.get());
    if (!zip_mount_file.empty())
      manifest->zip_mount_file = strdup(zip_mount_file.c_str());
  }

  // write pkgid for recovery file
  if (context_->recovery_info.get().recovery_file) {
    context_->recovery_info.get().recovery_file->set_pkgid(manifest->package);
    context_->recovery_info.get().recovery_file->WriteAndCommitFileContent();
  }

  LOG(DEBUG) << "Parsed package id: " << info->package();

  switch (store_location_) {
    case StoreLocation::NORMAL:
      context_->manifest_data.set(manifest);
      break;
    case StoreLocation::BACKUP:
      context_->old_manifest_data.set(manifest);
      break;
    default:
      LOG(ERROR) << "Unknown store location for parsed data";
      return Step::Status::ERROR;
  }
  return Step::Status::OK;
}

}  // namespace configuration
}  // namespace common_installer
