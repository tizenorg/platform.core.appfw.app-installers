// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "tpk/step/step_parse.h"

#include <tpk_manifest_handlers/account_handler.h>
#include <tpk_manifest_handlers/application_manifest_constants.h>
#include <tpk_manifest_handlers/author_handler.h>
#include <tpk_manifest_handlers/description_handler.h>
#include <tpk_manifest_handlers/package_handler.h>
#include <tpk_manifest_handlers/privileges_handler.h>
#include <tpk_manifest_handlers/service_application_handler.h>
#include <tpk_manifest_handlers/ui_application_handler.h>
#include <manifest_parser/manifest_constants.h>

#include <pkgmgr/pkgmgr_parser.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <set>
#include <type_traits>
#include <string>
#include <vector>

#include "common/app_installer.h"
#include "common/installer_context.h"
#include "common/step/step.h"
#include "utils/clist_helpers.h"

namespace tpk {
namespace parse {

namespace app_keys = tpk::application_keys;
namespace bf = boost::filesystem;
namespace manifest_keys = tpk::manifest_keys;

common_installer::Step::Status StepParse::precheck() {
  if (context_->unpacked_dir_path.get().empty()) {
    LOG(ERROR) << "unpacked_dir_path attribute is empty";
    return common_installer::Step::Status::INVALID_VALUE;
  }
  if (!boost::filesystem::exists(context_->unpacked_dir_path.get())) {
    LOG(ERROR) << "unpacked_dir_path ("
               << context_->unpacked_dir_path.get()
               << ") path does not exist";
    return common_installer::Step::Status::INVALID_VALUE;
  }

  boost::filesystem::path tmp(context_->unpacked_dir_path.get());
  tmp /= manifest_keys::kManifestFileName;

  if (!boost::filesystem::exists(tmp)) {
    LOG(ERROR) << manifest_keys::kManifestFileName
               << " not found from the package";
    return common_installer::Step::Status::INVALID_VALUE;
  }

  return common_installer::Step::Status::OK;
}

bool StepParse::LocateConfigFile() {
  boost::filesystem::path manifest = context_->unpacked_dir_path.get();
  manifest /= manifest_keys::kManifestFileName;

  LOG(DEBUG) << "tizen_manifest.xml path: " << manifest;

  if (!boost::filesystem::exists(manifest))
    return false;

  path_ = manifest;
  return true;
}

bf::path StepParse::LocateConfigFile() const {
  boost::filesystem::path path(context_->unpacked_dir_path.get());
  path /= manifest_keys::kManifestFileName;
  return path;
}

bool StepParse::FillPackageInfo(manifest_x* manifest) {
  std::shared_ptr<const PackageInfo> app_info =
      std::static_pointer_cast<const PackageInfo>(
          parser_->GetManifestData(manifest_keys::kManifestKey));
  if (!app_info) {
    LOG(ERROR) << "Application info manifest data has not been found.";
    return false;
  }

  std::shared_ptr<const UIApplicationInfoList> ui_application_list =
      std::static_pointer_cast<const UIApplicationInfoList>(
          parser_->GetManifestData(app_keys::kUIApplicationKey));
  std::shared_ptr<const ServiceApplicationInfoList> service_application_list =
      std::static_pointer_cast<const ServiceApplicationInfoList>(
          parser_->GetManifestData(app_keys::kServiceApplicationKey));

  // mandatory check
  if (!ui_application_list && !service_application_list) {
    LOG(ERROR) << "UI Application or Service Application "
                  "are mandatory and has not been found.";
    return false;
  }

  manifest->package = strdup(app_info->package().c_str());
  manifest->type = strdup("tpk");
  manifest->version = strdup(app_info->version().c_str());
  manifest->installlocation = strdup(app_info->install_location().c_str());
  manifest->api_version = strdup(app_info->api_version().c_str());

  if (ui_application_list) {
    manifest->mainapp_id =
        strdup(ui_application_list->items[0].ui_info.appid().c_str());
  } else {
    manifest->mainapp_id =
        strdup(service_application_list->items[0].sa_info.appid().c_str());
  }
  return true;
}

bool StepParse::FillAuthorInfo(manifest_x* manifest) {
  std::shared_ptr<const AuthorInfo> author_info =
      std::static_pointer_cast<const AuthorInfo>(
          parser_->GetManifestData(app_keys::kAuthorKey));

  if (!author_info) {
    LOG(ERROR) << "Author data has not been found.";
    return false;
  }

  author_x* author = reinterpret_cast<author_x*>(calloc(1, sizeof(author_x)));
  author->text = strdup(author_info->name().c_str());
  author->email = strdup(author_info->email().c_str());
  author->href = strdup(author_info->href().c_str());
  LISTADD(manifest->author, author);
  return true;
}

bool StepParse::FillDescription(manifest_x* manifest) {
  std::shared_ptr<const DescriptionInfo> description_info =
      std::static_pointer_cast<const DescriptionInfo>(
          parser_->GetManifestData(app_keys::kDescriptionKey));

  if (!description_info) {
    LOG(ERROR) << "Description data has not been found.";
    return false;
  }

  description_x* description = reinterpret_cast<description_x*>
      (calloc(1, sizeof(description_x)));
  description->text = strdup(description_info->description().c_str());
  description->lang = strdup(description_info->xml_lang().c_str());
  LISTADD(manifest->description, description);
  return true;
}

bool StepParse::FillPrivileges(manifest_x* manifest) {
  std::shared_ptr<const PrivilegesInfo> perm_info =
      std::static_pointer_cast<const PrivilegesInfo>(parser_->GetManifestData(
          app_keys::kPrivilegesKey));
  if (!perm_info)
    return true;

  std::set<std::string> privileges = perm_info->GetPrivileges();
  if (!privileges.empty()) {
    privileges_x* privileges_x_list =
        reinterpret_cast<privileges_x*>(calloc(1, sizeof(privileges_x)));
    manifest->privileges = privileges_x_list;
    for (const std::string& p : privileges) {
      privilege_x* privilege_x_node =
          reinterpret_cast<privilege_x*>(calloc(1, sizeof(privilege_x)));
      privilege_x_node->text = strdup(p.c_str());
      LISTADD(manifest->privileges->privilege, privilege_x_node);
    }
  }
  return true;
}

bool StepParse::FillServiceApplication(manifest_x* manifest) {
  std::shared_ptr<const ServiceApplicationInfoList> service_application_list =
      std::static_pointer_cast<const ServiceApplicationInfoList>(
          parser_->GetManifestData(app_keys::kServiceApplicationKey));
  if (!service_application_list)
    return true;

  for (const auto& application : service_application_list->items) {
    serviceapplication_x* service_app =
                          static_cast<serviceapplication_x*>
                          (calloc(1, sizeof(serviceapplication_x)));
    service_app->appid = strdup(application.sa_info.appid().c_str());
    service_app->autorestart =
        strdup(application.sa_info.auto_restart().c_str());
    service_app->exec = strdup(application.sa_info.exec().c_str());
    service_app->onboot = strdup(application.sa_info.on_boot().c_str());
    service_app->type = strdup(application.sa_info.type().c_str());
    LISTADD(manifest->serviceapplication, service_app);

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
  }
  return true;
}

bool StepParse::FillUIApplication(manifest_x* manifest) {
  std::shared_ptr<const UIApplicationInfoList> ui_application_list =
      std::static_pointer_cast<const UIApplicationInfoList>(
          parser_->GetManifestData(app_keys::kUIApplicationKey));
  if (!ui_application_list)
    return true;

  for (const auto& application : ui_application_list->items) {
    uiapplication_x* ui_app =
                       static_cast<uiapplication_x*>
                       (calloc(1, sizeof(uiapplication_x)));
    ui_app->appid = strdup(application.ui_info.appid().c_str());
    ui_app->exec = strdup(application.ui_info.exec().c_str());
    ui_app->multiple = strdup(application.ui_info.multiple().c_str());
    ui_app->nodisplay = strdup(application.ui_info.nodisplay().c_str());
    ui_app->taskmanage = strdup(application.ui_info.taskmanage().c_str());
    ui_app->type = strdup(application.ui_info.type().c_str());
    LISTADD(manifest->uiapplication, ui_app);

    if (!FillAppControl(ui_app, application.app_control))
      return false;
    if (!FillDataControl(ui_app, application.data_control))
      return false;
    if (!FillApplicationIconPaths(ui_app, application.app_icons))
      return false;
    if (!FillLabel(ui_app, application.label))
      return false;
    if (!FillMetadata(ui_app, application.meta_data))
      return false;
  }
  return true;
}

template <typename T1, typename T2>
bool StepParse::FillAppControl(T1* app, const T2& app_control_list) {
  static_assert(
      std::is_same<typename std::remove_pointer<T1>::type,
                   uiapplication_x>::value ||
      std::is_same<typename std::remove_pointer<T1>::type,
                   serviceapplication_x>::value,
      "T1 should be uiapplication_x or serviceapplication_x");
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
    LISTADD(app->appcontrol, app_control);
  }
  return true;
}

template <typename T1, typename T2>
bool StepParse::FillDataControl(T1* app, const T2& data_control_list) {
  static_assert(
      std::is_same<typename std::remove_pointer<T1>::type,
                   uiapplication_x>::value ||
      std::is_same<typename std::remove_pointer<T1>::type,
                   serviceapplication_x>::value,
      "T1 should be uiapplication_x or serviceapplication_x");
  if (data_control_list.empty())
    return true;

  for (const auto& control : data_control_list) {
    datacontrol_x* data_control =
          static_cast<datacontrol_x*>(calloc(1, sizeof(datacontrol_x)));
    data_control->access = strdup(control.access().c_str());
    data_control->providerid = strdup(control.providerid().c_str());
    data_control->type = strdup(control.type().c_str());
    LISTADD(app->datacontrol, data_control);
  }
  return true;
}

template <typename T1, typename T2>
bool StepParse::FillApplicationIconPaths(T1* app, const T2& icons_info) {
  static_assert(
      std::is_same<typename std::remove_pointer<T1>::type,
                   uiapplication_x>::value ||
      std::is_same<typename std::remove_pointer<T1>::type,
                   serviceapplication_x>::value,
      "T1 should be uiapplication_x or serviceapplication_x");
  for (auto& application_icon : icons_info.icons()) {
    icon_x* icon = reinterpret_cast<icon_x*> (calloc(1, sizeof(icon_x)));
    // NOTE: name is an attribute, but the xml writer uses it as text.
    // This must be fixed in whole app-installer modules, including wgt.
    // Current implementation is just for compatibility.
    icon->text = strdup(application_icon.path().c_str());
    icon->name = strdup(application_icon.path().c_str());
    LISTADD(app->icon, icon);
  }
  return true;
}

template <typename T1, typename T2>
bool StepParse::FillLabel(T1* app, const T2& label_list) {
  static_assert(
      std::is_same<typename std::remove_pointer<T1>::type,
                   uiapplication_x>::value ||
      std::is_same<typename std::remove_pointer<T1>::type,
                   serviceapplication_x>::value,
      "T1 should be uiapplication_x or serviceapplication_x");
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
    label->lang = strdup(control.xml_lang().c_str());
    LISTADD(app->label, label);
  }
  return true;
}

template <typename T1, typename T2>
bool StepParse::FillMetadata(T1* app, const T2& meta_data_list) {
  static_assert(
      std::is_same<typename std::remove_pointer<T1>::type,
                   uiapplication_x>::value ||
      std::is_same<typename std::remove_pointer<T1>::type,
                   serviceapplication_x>::value,
      "T1 should be uiapplication_x or serviceapplication_x");
  if (meta_data_list.empty())
    return true;

  for (auto& meta : meta_data_list) {
    metadata_x* metadata =
        static_cast<metadata_x*>(calloc(1, sizeof(metadata_x)));
    metadata->key = strdup(meta.key().c_str());
    metadata->value = strdup(meta.val().c_str());
    LISTADD(app->metadata, metadata);
  }
  return true;
}

bool StepParse::FillAccounts() {
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
    single_info.names = account.labels;
    // appid has the same value as package
    single_info.appid =  account.app_id;
    single_info.providerid = account.provider_id;
    info.set_account(single_info);
  }
  context_->manifest_plugins_data.get().account_info.set(info);
  return true;
}

bool StepParse::FillManifestX(manifest_x* manifest) {
  if (!FillPackageInfo(manifest))
    return false;
  if (!FillUIApplication(manifest))
    return false;
  if (!FillServiceApplication(manifest))
    return false;
  if (!FillPrivileges(manifest))
    return false;
  if (!FillAccounts())
    return false;
  return true;
}

common_installer::Step::Status StepParse::process() {
  if (!LocateConfigFile()) {
    LOG(ERROR) << "No tizen_manifest.xml";
    return common_installer::Step::Status::ERROR;
  }
  parser_.reset(new tpk::parse::TPKConfigParser());
  if (!parser_->ParseManifest(path_)) {
    LOG(ERROR) << "[Parse] Parse failed. " <<  parser_->GetErrorMessage();
    return common_installer::Step::Status::ERROR;
  }

  const manifest_x* manifest = context_->manifest_data.get();
  if (!FillManifestX(const_cast<manifest_x*>(manifest))) {
    LOG(ERROR) << "[Parse] Storing manifest_x failed. "
               <<  parser_->GetErrorMessage();
    return common_installer::Step::Status::ERROR;
  }

  // Copy data from ManifestData to InstallerContext
  std::shared_ptr<const PackageInfo> info =
      std::static_pointer_cast<const PackageInfo>(
          parser_->GetManifestData(
              manifest_keys::kManifestKey));

  context_->pkgid.set(manifest->package);

  // write pkgid for recovery file
  if (context_->recovery_info.get().recovery_file) {
    context_->recovery_info.get().recovery_file->set_pkgid(manifest->package);
    context_->recovery_info.get().recovery_file->WriteAndCommitFileContent();
  }

  std::shared_ptr<const PrivilegesInfo> perm_info =
      std::static_pointer_cast<const PrivilegesInfo>(
          parser_->GetManifestData(
              application_keys::kPrivilegesKey));
  parser::PrivilegesSet privileges;
  if (perm_info)
    privileges = perm_info->GetPrivileges();

  LOG(DEBUG) << " Read data -[ ";
  LOG(DEBUG) << "App package: " << info->package();
  LOG(DEBUG) << "  aplication version     = " <<  info->version();
  LOG(DEBUG) << "  api_version = " <<  info->api_version();
  LOG(DEBUG) << "  privileges -[";
  for (const auto& p : privileges) {
    LOG(DEBUG) << "    " << p;
  }
  LOG(DEBUG) << "  ]-";
  LOG(DEBUG) << "]-";

  return common_installer::Step::Status::OK;
}

}  // namespace parse
}  // namespace tpk
