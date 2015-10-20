/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_parse.h"

#include <manifest_handlers/account_handler.h>
#include <manifest_handlers/app_control_handler.h>
#include <manifest_handlers/application_icons_handler.h>
#include <manifest_handlers/application_manifest_constants.h>
#include <manifest_handlers/category_handler.h>
#include <manifest_handlers/content_handler.h>
#include <manifest_handlers/metadata_handler.h>
#include <manifest_handlers/service_handler.h>
#include <manifest_handlers/setting_handler.h>
#include <manifest_handlers/tizen_application_handler.h>
#include <manifest_handlers/widget_handler.h>

#include <pkgmgr/pkgmgr_parser.h>

#include <string.h>

#include <cstdio>
#include <cstdlib>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "common/app_installer.h"
#include "common/installer_context.h"
#include "common/step/step.h"
#include "utils/clist_helpers.h"
#include "wgt/wgt_backend_data.h"

namespace {

const std::string kManifestVersion = "1.0.0";

metadata_x* GenerateMetadataListX(const wgt::parse::MetaDataInfo& meta_info,
    metadata_x* head) {
  for (auto& meta : meta_info.metadata()) {
    metadata_x* new_meta =
        static_cast<metadata_x*>(calloc(1, sizeof(metadata_x)));
    new_meta->key = strdup(meta.first.c_str());
    if (!meta.second.empty())
      new_meta->value = strdup(meta.second.c_str());
    LISTADD(head, new_meta);
  }
  return head;
}

}  // namespace

namespace wgt {
namespace parse {

namespace app_keys = wgt::application_widget_keys;

StepParse::StepParse(common_installer::InstallerContext* context,
                     bool check_start_file)
    : Step(context),
      check_start_file_(check_start_file) {
}

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
          parser_->GetManifestData(app_keys::kIconsKey));
  if (icons_info.get()) {
    for (auto& application_icon : icons_info->icons()) {
      icon_x* icon = reinterpret_cast<icon_x*> (calloc(1, sizeof(icon_x)));
      icon->text = strdup(application_icon.path().c_str());
      LISTADD(manifest->icon, icon);
    }
  }
  return true;
}

bool StepParse::FillWidgetInfo(manifest_x* manifest) {
  std::shared_ptr<const WidgetInfo> wgt_info =
      std::static_pointer_cast<const WidgetInfo>(parser_->GetManifestData(
          app_keys::kWidgetKey));
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

  // For wgt package use the long name
  for (auto& item : wgt_info->name_set()) {
    label_x* label = reinterpret_cast<label_x*>(calloc(1, sizeof(label_x)));
    label->name = strdup(item.second.c_str());
    label->lang = strdup(item.first.c_str());
    LISTADD(manifest->application->label, label);
  }

  author_x* author = reinterpret_cast<author_x*>(calloc(1, sizeof(author_x)));
  if (!wgt_info->author().empty())
    author->text = strdup(wgt_info->author().c_str());
  if (!wgt_info->author_email().empty())
    author->email = strdup(wgt_info->author_email().c_str());
  if (!wgt_info->author_href().empty())
    author->href = strdup(wgt_info->author_href().c_str());
  LISTADD(manifest->author, author);

  std::shared_ptr<const SettingInfo> settings_info =
      std::static_pointer_cast<const SettingInfo>(
          parser_->GetManifestData(
              wgt::application_widget_keys::kTizenSettingKey));
  if (settings_info) {
    switch (settings_info->install_location()) {
    case wgt::parse::SettingInfo::InstallLocation::AUTO: {
      manifest->installlocation = strdup("auto");
      break;
    }
    case wgt::parse::SettingInfo::InstallLocation::INTERNAL: {
      manifest->installlocation = strdup("internal-only");
      break;
    }
    case wgt::parse::SettingInfo::InstallLocation::EXTERNAL: {
      manifest->installlocation = strdup("prefer-external");
      break;
    }
    }
  } else {
    manifest->installlocation = strdup("auto");
  }

  return true;
}

bool StepParse::FillUIApplicationInfo(manifest_x* manifest) {
  std::shared_ptr<const TizenApplicationInfo> app_info =
      std::static_pointer_cast<const TizenApplicationInfo>(
          parser_->GetManifestData(app_keys::kTizenApplicationKey));
  if (!app_info) {
    LOG(ERROR) << "Application info manifest data has not been found.";
    return false;
  }
  // application data
  manifest->application = reinterpret_cast<application_x*>
    (calloc (1, sizeof(application_x)));
  manifest->application->component_type = strdup("uiapp");
  manifest->application->icon =
      reinterpret_cast<icon_x*> (calloc(1, sizeof(icon_x)));
  manifest->application->appid = strdup(app_info->id().c_str());
  manifest->application->type = strdup("webapp");
  manifest->package = strdup(app_info->package().c_str());
  manifest->mainapp_id = strdup(app_info->id().c_str());
  if (manifest->icon) {
    icon_x* icon = nullptr;
    LISTHEAD(manifest->icon, icon);
    manifest->application->icon->text = strdup(icon->text);
  }

  return true;
}

bool StepParse::FillServiceApplicationInfo(manifest_x* manifest) {
  std::shared_ptr<const ServiceList> service_list =
      std::static_pointer_cast<const ServiceList>(
          parser_->GetManifestData(app_keys::kTizenServiceKey));
  if (!service_list)
    return true;
  for (auto& service_info : service_list->services) {
    application_x* application = reinterpret_cast<application_x*>
        (calloc(1, sizeof(application_x)));
    application->component_type = strdup("svcapp");
    application->appid = strdup(service_info.id().c_str());
    application->type = strdup("webapp");
    application->onboot =
        service_info.on_boot() ? strdup("true") : strdup("false");
    application->autorestart =
        service_info.auto_restart() ? strdup("true") : strdup("false");

    for (auto& pair : service_info.names()) {
      label_x* label = reinterpret_cast<label_x*>(calloc(1, sizeof(label_x)));
      if (!pair.first.empty())
        label->lang = strdup(pair.first.c_str());
      label->name = strdup(pair.second.c_str());
      LISTADD(application->label, label);
    }

    if (!service_info.icon().empty()) {
      icon_x* icon = reinterpret_cast<icon_x*>(calloc(1, sizeof(icon_x)));
      icon->text = strdup(service_info.icon().c_str());
      application->icon = icon;
    }

    // TODO(t.iwanek): what about description, how is it different from name?

    for (auto& category : service_info.categories()) {
      category_x* c = reinterpret_cast<category_x*>(
          calloc(1, sizeof(category_x)));
      c->name = strdup(category.c_str());
      LISTADD(application->category, c);
    }

    for (auto& pair : service_info.metadata_set()) {
      metadata_x* item = reinterpret_cast<metadata_x*>(
          calloc(1, sizeof(metadata_x)));
      item->key = strdup(pair.first.c_str());
      if (!pair.second.empty())
        item->value = strdup(pair.second.c_str());
      LISTADD(application->metadata, item);
    }

    LISTADD(manifest->application, application);
  }
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
      LISTADD(manifest->application->appcontrol, app_control);
    }
  }
  return true;
}

bool StepParse::FillPrivileges(manifest_x* manifest) {
  std::shared_ptr<const PermissionsInfo> perm_info =
      std::static_pointer_cast<const PermissionsInfo>(parser_->GetManifestData(
          app_keys::kTizenPermissionsKey));
  std::set<std::string> privileges;
  if (perm_info)
    privileges = ExtractPrivileges(perm_info);

  if (!privileges.empty()) {
    privileges_x* privileges_x_list =
        reinterpret_cast<privileges_x*>(calloc(1, sizeof(privileges_x)));
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

bool StepParse::FillCategories(manifest_x* manifest) {
  std::shared_ptr<const CategoryInfoList> category_info =
      std::static_pointer_cast<const CategoryInfoList>(parser_->GetManifestData(
          app_keys::kTizenCategoryKey));
  if (!category_info)
    return true;

  // there is one app atm
  for (auto& category : category_info->categories) {
    category_x* c = reinterpret_cast<category_x*>(
        calloc(1, sizeof(category_x)));
    c->name = strdup(category.c_str());
    LISTADD(manifest->application->category, c);
  }
  return true;
}

bool StepParse::FillMetadata(manifest_x* manifest) {
  std::shared_ptr<const MetaDataInfo> meta_info =
      std::static_pointer_cast<const MetaDataInfo>(parser_->GetManifestData(
          app_keys::kTizenMetaDataKey));
  if (!meta_info)
    return true;

  application_x* app = nullptr;
  PKGMGR_LIST_MOVE_NODE_TO_HEAD(manifest->application, app);
  for (; app; app = app->next) {
    manifest->application->metadata =
        GenerateMetadataListX(*meta_info, manifest->application->metadata);
  }
  return true;
}

bool StepParse::FillAccounts(manifest_x* manifest) {
  std::shared_ptr<const AccountInfo> account_info =
      std::static_pointer_cast<const AccountInfo>(parser_->GetManifestData(
          app_keys::kAccountKey));
  if (!account_info)
    return true;
  common_installer::AccountInfo info;
  for (auto& account : account_info->accounts()) {
    common_installer::SingleAccountInfo single_info;
    single_info.capabilities = account.capabilities;
    single_info.icon_paths = account.icon_paths;
    single_info.multiple_account_support = account.multiple_account_support;
    single_info.names = account.names;
    // wgt can contain only one app so this assumes mainapp_id is valid here
    single_info.appid = manifest->mainapp_id;
    info.set_account(single_info);
  }
  context_->manifest_plugins_data.get().account_info.set(info);
  return true;
}

bool StepParse::FillExtraManifestInfo(manifest_x* manifest) {
  return FillAccounts(manifest);
}

bool StepParse::FillManifestX(manifest_x* manifest) {
  if (!FillIconPaths(manifest))
    return false;
  if (!FillUIApplicationInfo(manifest))
    return false;
  if (!FillWidgetInfo(manifest))
    return false;
  if (!FillPrivileges(manifest))
    return false;
  if (!FillAppControl(manifest))
    return false;
  if (!FillCategories(manifest))
    return false;
  if (!FillMetadata(manifest))
    return false;
  // TODO(t.iwanek): fix adding ui application element
  // for now adding application service is added here because rest of code
  // assumes that there is one application at manifest->application
  // so this must execute last
  if (!FillServiceApplicationInfo(manifest))
    return false;
  if (!FillExtraManifestInfo(manifest))
    return false;
  return true;
}

bool StepParse::LocateConfigFile() {
  return StepParse::Check(context_->unpacked_dir_path.get());
}

common_installer::Step::Status StepParse::process() {
  if (!LocateConfigFile()) {
    LOG(ERROR) << "No config.xml";
    return common_installer::Step::Status::ERROR;
  }

  parser_.reset(new wgt::parse::WidgetConfigParser());
  if (!parser_->ParseManifest(config_)) {
    LOG(ERROR) << "[Parse] Parse failed. " <<  parser_->GetErrorMessage();
    return common_installer::Step::Status::ERROR;
  }
  if (check_start_file_) {
    if (!parser_->HasValidStartFile()) {
      LOG(ERROR) << parser_->GetErrorMessage();
      return common_installer::Step::Status::ERROR;
    }
    if (!parser_->HasValidServicesStartFiles()) {
      LOG(ERROR) << parser_->GetErrorMessage();
      return common_installer::Step::Status::ERROR;
    }
  }

  manifest_x* manifest = context_->manifest_data.get();
  if (!FillManifestX(manifest)) {
    LOG(ERROR) << "[Parse] Storing manifest_x failed. "
               <<  parser_->GetErrorMessage();
    return common_installer::Step::Status::ERROR;
  }

  // Copy data from ManifestData to InstallerContext
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

  const std::string& package_version = wgt_info->version();
  const std::string& required_api_version = info->required_version();

  context_->manifest_data.get()->api_version =
      strdup(required_api_version.c_str());
  context_->pkgid.set(manifest->package);

  // write pkgid for recovery file
  if (context_->recovery_info.get().recovery_file) {
    context_->recovery_info.get().recovery_file->set_pkgid(manifest->package);
    context_->recovery_info.get().recovery_file->WriteAndCommitFileContent();
  }

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
  LOG(DEBUG) << "  aplication version     = " <<  package_version;
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
