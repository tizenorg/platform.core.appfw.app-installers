// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache 2.0 license that can be
// found in the LICENSE file.

#include "tpk/step/step_parse.h"

#include <tpk_manifest_handlers/account_handler.h>
#include <tpk_manifest_handlers/app_control_handler.h>
#include <tpk_manifest_handlers/application_icons_handler.h>
#include <tpk_manifest_handlers/application_manifest_constants.h>
#include <tpk_manifest_handlers/author_handler.h>
#include <tpk_manifest_handlers/datacontrol_handler.h>
#include <tpk_manifest_handlers/description_handler.h>
#include <tpk_manifest_handlers/label_handler.h>
#include <tpk_manifest_handlers/metadata_handler.h>
#include <tpk_manifest_handlers/privileges_handler.h>
#include <tpk_manifest_handlers/service_application_handler.h>
#include <tpk_manifest_handlers/tizen_application_handler.h>
#include <tpk_manifest_handlers/ui_application_handler.h>
#include <manifest_parser/manifest_constants.h>

#include <pkgmgr/pkgmgr_parser.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "common/app_installer.h"
#include "common/context_installer.h"
#include "common/step/step.h"
#include "utils/clist_helpers.h"

namespace tpk {
namespace parse {

namespace {

metadata_x* GenerateMetadataListX(const tpk::parse::MetaDataInfoList& meta_info,
    metadata_x* head) {
  for (auto& meta : meta_info.items) {
    metadata_x* new_meta =
        static_cast<metadata_x*>(calloc(1, sizeof(metadata_x)));
    new_meta->key = strdup(meta.key().c_str());
    if (!meta.key().empty())
      new_meta->value = strdup(meta.val().c_str());
    LISTADD(head, new_meta);
  }
  return head;
}

}  // namespace

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

StepParse::StepParse(common_installer::ContextInstaller* context,
                     bool check_start_file)
    : Step(context),
      check_start_file_(check_start_file) {
}

// Locating tizen-manifest.xml file
bool StepParse::LocateConfigFile() {
  return StepParse::Check(context_->unpacked_dir_path.get());
}

// Function neded by step_recovery
bf::path StepParse::LocateConfigFile() const {
  boost::filesystem::path path(context_->unpacked_dir_path.get());
  path /= manifest_keys::kManifestFileName;
  return path;
}

// Extracting privileges
std::set<std::string> StepParse::ExtractPrivileges(
    std::shared_ptr<const PrivilegesInfo> perm_info) const {
  return perm_info->GetPrivileges();
}

// icon
template <typename T>
bool StepParse::FillIconPaths(T* manifest) {
  std::shared_ptr<const ApplicationIconsInfo> icons_info =
      std::static_pointer_cast<const ApplicationIconsInfo>(
          parser_->GetManifestData(app_keys::kIconKey));
  if (!icons_info) {
    LOG(ERROR) << "Application icon has not been found.";
    return false;
  }

  for (auto& application_icon : icons_info->icons()) {
    icon_x* icon = reinterpret_cast<icon_x*> (calloc(1, sizeof(icon_x)));
      // NOTE: name is an attribute, but the xml writer uses it as text.
      // This must be fixed in whole app-installer modules, including wgt.
      // Current implementation is just for compatibility.
      icon->text = strdup(application_icon.path().c_str());
      icon->name = strdup(application_icon.path().c_str());
    LISTADD(manifest, icon);
  }
  return true;
}

// application info
bool StepParse::FillApplicationInfo(manifest_x* manifest) {
  std::shared_ptr<const TizenApplicationInfo> app_info =
      std::static_pointer_cast<const TizenApplicationInfo>(
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

  manifest->uiapplication = reinterpret_cast<uiapplication_x*>
    (calloc (1, sizeof(uiapplication_x)));
  manifest->uiapplication->icon =
      reinterpret_cast<icon_x*> (calloc(1, sizeof(icon_x)));

  manifest->package = strdup(app_info->package().c_str());
  manifest->type = strdup("tpk");
  manifest->version = strdup(app_info->version().c_str());
  manifest->installlocation = strdup(app_info->install_location().c_str());
  manifest->api_version = strdup(app_info->api_version().c_str());

  if (manifest->icon) {
    icon_x* icon = nullptr;
    LISTHEAD(manifest->icon, icon);
    manifest->uiapplication->icon->name = strdup(icon->name);
  }
  manifest->uiapplication->next = nullptr;

  if (ui_application_list) {
    manifest->mainapp_id =
                strdup(ui_application_list->items[0].appid().c_str());
  } else {
    manifest->mainapp_id =
                strdup(service_application_list->items[0].appid().c_str());
  }

  if (!FillLabel(manifest->label))
    return false;

  return true;
}

// author
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

// description
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

// privileges
bool StepParse::FillPrivileges(manifest_x* manifest) {
  std::shared_ptr<const PrivilegesInfo> perm_info =
      std::static_pointer_cast<const PrivilegesInfo>(parser_->GetManifestData(
          app_keys::kPrivilegeKey));
  std::set<std::string> privileges;

  if (perm_info)
    privileges = ExtractPrivileges(perm_info);

  if (!privileges.empty()) {
    privileges_x* privileges_x_list =
        reinterpret_cast<privileges_x*> (calloc(1, sizeof(privileges_x)));
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

// service application
bool StepParse::FillServiceApplication(manifest_x* manifest) {
  std::shared_ptr<const ServiceApplicationInfoList> service_application_list =
      std::static_pointer_cast<const ServiceApplicationInfoList>(
          parser_->GetManifestData(app_keys::kServiceApplicationKey));

  if (!service_application_list) {
    LOG(ERROR) << "Service Application data has not been found.";
    return false;
  }

  for (const auto& control : service_application_list->items) {
    serviceapplication_x* service_app =
                          static_cast<serviceapplication_x*>
                          (calloc(1, sizeof(serviceapplication_x)));
      service_app->appid = strdup(control.appid().c_str());
      service_app->autorestart = strdup(control.auto_restart().c_str());
      service_app->exec = strdup(control.exec().c_str());
      service_app->onboot = strdup(control.on_boot().c_str());
      service_app->type = strdup(control.type().c_str());
    LISTADD(manifest->serviceapplication, service_app);

    if (!FillAppControl(manifest->uiapplication->appcontrol))
      return false;
    if (!FillDataControl(manifest->uiapplication->datacontrol))
      return false;
    if (!FillIconPaths(manifest->uiapplication->icon))
      return false;
    if (!FillLabel(manifest->uiapplication->label))
      return false;
  }
  return true;
}

// ui application
bool StepParse::FillUIApplication(manifest_x* manifest) {
  std::shared_ptr<const UIApplicationInfoList> ui_application_list =
      std::static_pointer_cast<const UIApplicationInfoList>(
          parser_->GetManifestData(app_keys::kUIApplicationKey));

  if (!ui_application_list) {
    LOG(ERROR) << "UI Application data has not been found.";
    return false;
  }

  for (const auto& control : ui_application_list->items) {
    uiapplication_x* ui_app =
                       static_cast<uiapplication_x*>
                       (calloc(1, sizeof(uiapplication_x)));
      ui_app->appid = strdup(control.appid().c_str());
      ui_app->exec = strdup(control.exec().c_str());
      ui_app->multiple = strdup(control.multiple().c_str());
      ui_app->nodisplay = strdup(control.nodisplay().c_str());
      ui_app->taskmanage = strdup(control.taskmanage().c_str());
      ui_app->type = strdup(control.type().c_str());
    LISTADD(manifest->uiapplication, ui_app);

    if (!FillAppControl(manifest->serviceapplication->appcontrol))
      return false;
    if (!FillDataControl(manifest->serviceapplication->datacontrol))
      return false;
    if (!FillIconPaths(manifest->serviceapplication->icon))
      return false;
    if (!FillLabel(manifest->serviceapplication->label))
      return false;
  }
  return true;
}

// app_control
template <typename T>
bool StepParse::FillAppControl(T* manifest) {
  std::shared_ptr<const AppControlInfoList> app_control_list =
      std::static_pointer_cast<const AppControlInfoList>(
          parser_->GetManifestData(app_keys::kAppControlKey));

  if (!app_control_list) {
    LOG(ERROR) << "App Control data has not been found.";
    return false;
  }

  for (const auto& control : app_control_list->items) {
    appcontrol_x* app_control =
          static_cast<appcontrol_x*>(calloc(1, sizeof(appcontrol_x)));
      app_control->operation = strdup(control.operation().c_str());
      app_control->mime = strdup(control.mime().c_str());
      app_control->uri = strdup(control.uri().c_str());
    LISTADD(manifest, app_control);
  }
  return true;
}

// datacontrol
template <typename T>
bool StepParse::FillDataControl(T* manifest) {
  std::shared_ptr<const DataControlInfoList> data_control_list =
      std::static_pointer_cast<const DataControlInfoList>(
          parser_->GetManifestData(app_keys::kDataControlKey));

  if (!data_control_list) {
    LOG(ERROR) << "Data Control has not been found.";
    return false;
  }

  for (const auto& control : data_control_list->items) {
    datacontrol_x* data_control =
          static_cast<datacontrol_x*>(calloc(1, sizeof(datacontrol_x)));
      data_control->access = strdup(control.access().c_str());
      data_control->providerid = strdup(control.providerid().c_str());
      data_control->type = strdup(control.type().c_str());
    LISTADD(manifest, data_control);
  }
  return true;
}

// label
template <typename T>
bool StepParse::FillLabel(T* manifest) {
  std::shared_ptr<const LabelInfoList> label_list =
      std::static_pointer_cast<const LabelInfoList>(
          parser_->GetManifestData(app_keys::kLableKey));

  if (!label_list) {
    LOG(ERROR) << "Label data has not been found.";
    return false;
  }

  for (const auto& control : label_list->items) {
    label_x* label =
          static_cast<label_x*>(calloc(1, sizeof(label_x)));
      label->text = strdup(control.text().c_str());
      label->name = strdup(control.name().c_str());
      label->lang = strdup(control.xml_lang().c_str());
    LISTADD(manifest, label);
  }
  return true;
}

// metadata
bool StepParse::FillMetadata(manifest_x* manifest) {
  std::shared_ptr<const MetaDataInfoList> meta_info =
      std::static_pointer_cast<const MetaDataInfoList>(parser_->GetManifestData(
          app_keys::kMetaData));
  if (!meta_info) {
    LOG(ERROR) << "Meta Data has not been found.";
    return false;
  }

  uiapplication_x* ui_app = nullptr;
  PKGMGR_LIST_MOVE_NODE_TO_HEAD(manifest->uiapplication, ui_app);
  for (; ui_app; ui_app = ui_app->next) {
    manifest->uiapplication->metadata =
        GenerateMetadataListX(*meta_info, manifest->uiapplication->metadata);
  }
  serviceapplication_x* svc_app = nullptr;
  PKGMGR_LIST_MOVE_NODE_TO_HEAD(manifest->serviceapplication, svc_app);
  for (; svc_app; svc_app = svc_app->next) {
    manifest->serviceapplication->metadata =
        GenerateMetadataListX(*meta_info,
                              manifest->serviceapplication->metadata);
  }
  return true;
}

// account
bool StepParse::FillAccounts(void) {
  std::shared_ptr<const AccountInfo> account_info =
      std::static_pointer_cast<const AccountInfo>(parser_->GetManifestData(
          app_keys::kAccountKey));
  if (!account_info) {
    LOG(ERROR) << "Account Info has not been found.";
    return false;
  }

  common_installer::AccountInfo info;
  for (auto& account : account_info->accounts()) {
    common_installer::SingleAccountInfo single_info;
    single_info.capabilities = account.capabilities;
    single_info.icon_paths = account.icon_paths;
    single_info.multiple_account_support = account.multiple_account_support;
    single_info.names = account.labels;
    // appid has the same value as package
    single_info.appid =  account.app_id;
    info.set_account(single_info);
  }
    context_->manifest_plugins_data.get().account_info.set(info);
  return true;
}

bool StepParse::FillManifestX(manifest_x* manifest) {
  if (!FillIconPaths(manifest->icon))
    return false;
  if (!FillApplicationInfo(manifest))
    return false;
  if (!FillMetadata(manifest))
    return false;

  FillServiceApplication(manifest);
  FillUIApplication(manifest);
  FillPrivileges(manifest);
  FillAccounts();

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

  // Copy data from ManifestData to ContextInstaller
  std::shared_ptr<const TizenApplicationInfo> info =
      std::static_pointer_cast<const TizenApplicationInfo>(
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
              application_keys::kPrivilegeKey));
  parser::PrivilegesSet privileges;
  if (perm_info)
    privileges = perm_info->GetPrivileges();

  LOG(DEBUG) << " Read data -[ ";
  LOG(DEBUG) << "App package: " << info->package();
  LOG(DEBUG) << "  aplication version     = " <<  info->version();
  LOG(DEBUG) << "  icon        = " <<  manifest->uiapplication->icon->name;
  LOG(DEBUG) << "  api_version = " <<  info->api_version();
  LOG(DEBUG) << "  privileges -[";
  for (const auto& p : privileges) {
    LOG(DEBUG) << "    " << p;
  }
  LOG(DEBUG) << "  ]-";
  LOG(DEBUG) << "]-";

  return common_installer::Step::Status::OK;
}

bool StepParse::Check(const boost::filesystem::path& widget_path) {
  boost::filesystem::path manifest = widget_path;
  manifest /= manifest_keys::kManifestFileName;

  LOG(DEBUG) << "tizen_manifest.xml path: " << manifest;

  if (!boost::filesystem::exists(manifest))
    return false;

  path_ = manifest;
  return true;
}

}  // namespace parse
}  // namespace tpk
