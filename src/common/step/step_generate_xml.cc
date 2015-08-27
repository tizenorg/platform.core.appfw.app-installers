/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_generate_xml.h"

#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>
#include <libxml/parser.h>
#include <libxml/xmlreader.h>
#include <pkgmgr-info.h>
#include <pkgmgr_parser.h>
#include <tzplatform_config.h>
#include <unistd.h>

#include <cassert>
#include <cstring>
#include <string>

#include "common/utils/clist_helpers.h"
#include "common/utils/file_util.h"

namespace bs = boost::system;
namespace fs = boost::filesystem;

namespace common_installer {
namespace pkgmgr {

static void _writeUIApplicationAttributes(
    xmlTextWriterPtr writer, uiapplication_x */*app*/) {
    xmlTextWriterWriteAttribute(writer, BAD_CAST "taskmanage",
        BAD_CAST "true");
}

static void _writeServiceApplicationAttributes(
    xmlTextWriterPtr writer, serviceapplication_x *app) {
  xmlTextWriterWriteAttribute(writer, BAD_CAST "auto-restart",
      BAD_CAST(app->autorestart ? app->autorestart : "false"));
  xmlTextWriterWriteAttribute(writer, BAD_CAST "on-boot",
      BAD_CAST(app->onboot ? app->onboot : "false"));
  xmlTextWriterWriteAttribute(writer, BAD_CAST "permission-type",
      BAD_CAST(app->permission_type ? app->permission_type : ""));
}

template <typename T>
common_installer::Step::Status StepGenerateXml::GenerateApplicationCommonXml(
    T* app, xmlTextWriterPtr writer) {
  // common appributes among uiapplication_x and serviceapplication_x
  xmlTextWriterWriteAttribute(writer, BAD_CAST "appid", BAD_CAST app->appid);

  // binary is a symbolic link named <appid> and is located in <pkgid>/<appid>
  fs::path exec_path = context_->pkg_path.get()
      / fs::path("bin") / fs::path(app->appid);
  xmlTextWriterWriteAttribute(writer, BAD_CAST "exec",
                              BAD_CAST exec_path.string().c_str());
  if (app->type)
    xmlTextWriterWriteAttribute(writer, BAD_CAST "type", BAD_CAST app->type);
  else
    xmlTextWriterWriteAttribute(writer, BAD_CAST "type", BAD_CAST "capp");

  // app-specific attributes
  if (std::is_same<T, uiapplication_x>::value)
    _writeUIApplicationAttributes(
        writer, reinterpret_cast<uiapplication_x *>(app));
  if (std::is_same<T, serviceapplication_x>::value)
    _writeServiceApplicationAttributes(
        writer, reinterpret_cast<serviceapplication_x *>(app));
  if (app->label) {
    label_x* label = nullptr;
    LISTHEAD(app->label, label);
    for (; label; label = label->next) {
      xmlTextWriterStartElement(writer, BAD_CAST "label");
      if (label->lang && strlen(label->lang)) {
        xmlTextWriterWriteAttribute(writer, BAD_CAST "xml:lang",
                                    BAD_CAST label->lang);
      }
      xmlTextWriterWriteString(writer, BAD_CAST label->name);
      xmlTextWriterEndElement(writer);
    }
  }

  // icon is renamed to <appid.png>
  if (app->icon->text) {
    fs::path app_icon = context_->pkg_path.get() / "res/wgt" /
        app->icon->text;
    fs::path icon = app->appid;
    if (app_icon.has_extension())
      icon += app_icon.extension();
    else
      icon += fs::path(".png");

    if (fs::exists(app_icon)) {
      xmlTextWriterWriteFormatElement(writer, BAD_CAST "icon",
                                          "%s", BAD_CAST icon.c_str());
    }
  } else {
    // Default icon setting is role of the platform
    LOG(DEBUG) << "Icon was not found in package";
  }

  appcontrol_x* appc = nullptr;
  PKGMGR_LIST_MOVE_NODE_TO_HEAD(app->appcontrol, appc);
  for (; appc != nullptr; appc = appc->next) {
    xmlTextWriterStartElement(writer, BAD_CAST "app-control");

    if (appc->operation) {
      xmlTextWriterStartElement(writer, BAD_CAST "operation");
      xmlTextWriterWriteAttribute(writer, BAD_CAST "name",
          BAD_CAST appc->operation);
      xmlTextWriterEndElement(writer);
    }

    if (appc->uri) {
      xmlTextWriterStartElement(writer, BAD_CAST "uri");
      xmlTextWriterWriteAttribute(writer, BAD_CAST "name",
          BAD_CAST appc->uri);
      xmlTextWriterEndElement(writer);
    }

    if (appc->mime) {
      xmlTextWriterStartElement(writer, BAD_CAST "mime");
      xmlTextWriterWriteAttribute(writer, BAD_CAST "name",
          BAD_CAST appc->mime);
      xmlTextWriterEndElement(writer);
    }

    xmlTextWriterEndElement(writer);
  }

  metadata_x* meta = nullptr;
  PKGMGR_LIST_MOVE_NODE_TO_HEAD(app->metadata, meta);
  for (; meta; meta = meta->next) {
    xmlTextWriterStartElement(writer, BAD_CAST "metadata");
    xmlTextWriterWriteAttribute(writer, BAD_CAST "key",
        BAD_CAST meta->key);
    if (meta->value)
      xmlTextWriterWriteAttribute(writer, BAD_CAST "value",
          BAD_CAST meta->value);
    xmlTextWriterEndElement(writer);
  }

  return Step::Status::OK;
}

common_installer::Step::Status StepGenerateXml::precheck() {
  if (!context_->manifest_data.get()) {
    LOG(ERROR) << "manifest_data attribute is empty";
    return Step::Status::INVALID_VALUE;
  }
  if (context_->pkgid.get().empty()) {
    LOG(ERROR) << "pkgid attribute is empty";
    return Step::Status::INVALID_VALUE;   }

  if ((!context_->manifest_data.get()->uiapplication) &&
     (!context_->manifest_data.get()->serviceapplication)) {
    LOG(ERROR) << "There is neither UI applications nor"
               << "Services applications described!";
    return Step::Status::INVALID_VALUE;
  }
  // TODO(p.sikorski) check context_->uid.get()

  return Step::Status::OK;
}

common_installer::Step::Status StepGenerateXml::process() {
  fs::path xml_path = fs::path(getUserManifestPath(context_->uid.get()))
      / fs::path(context_->pkgid.get());
  xml_path += ".xml";
  context_->xml_path.set(xml_path.string());

  bs::error_code error;
  if (!fs::exists(xml_path.parent_path(), error)) {
    if (!common_installer::CreateDir(xml_path.parent_path())) {
      LOG(ERROR) <<
          "Directory for manifest xml is missing and cannot be created";
      return Status::ERROR;
    }
  }

  xmlTextWriterPtr writer;

  writer = xmlNewTextWriterFilename(context_->xml_path.get().c_str(), 0);
  if (!writer) {
    LOG(ERROR) << "Failed to create new file";
    return Step::Status::ERROR;
  }

  xmlTextWriterStartDocument(writer, nullptr, nullptr, nullptr);

  xmlTextWriterSetIndent(writer, 1);

  // add manifest Element
  xmlTextWriterStartElement(writer, BAD_CAST "manifest");

  xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns",
      BAD_CAST "http://tizen.org/ns/packages");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "package",
      BAD_CAST context_->manifest_data.get()->package);
  xmlTextWriterWriteAttribute(writer, BAD_CAST "type",
      BAD_CAST context_->manifest_data.get()->type);
  xmlTextWriterWriteAttribute(writer, BAD_CAST "version",
      BAD_CAST context_->manifest_data.get()->version);

  if (!context_->manifest_data.get()->label) {
    xmlTextWriterWriteFormatElement(writer, BAD_CAST "label",
        "%s", BAD_CAST "");
  } else {
    label_x* label = nullptr;
    LISTHEAD(context_->manifest_data.get()->label, label);
    for (; label; label = label->next) {
      xmlTextWriterStartElement(writer, BAD_CAST "label");
      if (label->lang && strlen(label->lang)) {
        xmlTextWriterWriteAttribute(writer, BAD_CAST "xml:lang",
                                    BAD_CAST label->lang);
      }
      xmlTextWriterWriteString(writer, BAD_CAST label->name);
      xmlTextWriterEndElement(writer);
    }
  }

  if (!context_->manifest_data.get()->author) {
    xmlTextWriterWriteFormatElement(writer, BAD_CAST "author",
        "%s", BAD_CAST "");
  } else {
    author_x* author = nullptr;
    LISTHEAD(context_->manifest_data.get()->author, author);
    for (; author; author = author->next) {
      xmlTextWriterStartElement(writer, BAD_CAST "author");
      if (author->email && strlen(author->email)) {
        xmlTextWriterWriteAttribute(writer, BAD_CAST "email",
                                    BAD_CAST author->email);
      }
      if (author->href && strlen(author->href)) {
        xmlTextWriterWriteAttribute(writer, BAD_CAST "href",
                                    BAD_CAST author->href);
      }
      xmlTextWriterWriteString(writer, BAD_CAST author->text);
      xmlTextWriterEndElement(writer);
    }
  }

  if (!context_->manifest_data.get()->description) {
    xmlTextWriterWriteFormatElement(writer, BAD_CAST "description",
        "%s", BAD_CAST "");
  } else {
    description_x* description = nullptr;
    LISTHEAD(context_->manifest_data.get()->description, description);
    for (; description; description = description->next) {
      xmlTextWriterStartElement(writer, BAD_CAST "description");
      if (description->lang && strlen(description->lang)) {
        xmlTextWriterWriteAttribute(writer, BAD_CAST "xml:lang",
                                    BAD_CAST description->lang);
      }
      xmlTextWriterWriteString(writer, BAD_CAST description->name);
      xmlTextWriterEndElement(writer);
    }
  }

  // add ui-application element per ui application
  uiapplication_x* ui = nullptr;
  PKGMGR_LIST_MOVE_NODE_TO_HEAD(context_->manifest_data.get()->uiapplication,
                                ui);
  for (; ui; ui = ui->next) {
    xmlTextWriterStartElement(writer, BAD_CAST "ui-application");
    GenerateApplicationCommonXml(ui, writer);
    xmlTextWriterEndElement(writer);
  }
  // add service-application element per service application
  serviceapplication_x* svc = nullptr;
  PKGMGR_LIST_MOVE_NODE_TO_HEAD(
      context_->manifest_data.get()->serviceapplication, svc);
  for (; svc; svc = svc->next) {
    xmlTextWriterStartElement(writer, BAD_CAST "service-application");
    GenerateApplicationCommonXml(svc, writer);
    xmlTextWriterEndElement(writer);
  }

  // add privilege element
  privileges_x* pvlg = nullptr;
  PKGMGR_LIST_MOVE_NODE_TO_HEAD(
      context_->manifest_data.get()->privileges,
      pvlg);
  for (; pvlg != nullptr; pvlg = pvlg->next) {
    xmlTextWriterStartElement(writer, BAD_CAST "privileges");
    privilege_x* pv = nullptr;
    PKGMGR_LIST_MOVE_NODE_TO_HEAD(pvlg->privilege, pv);
    for (; pv != nullptr; pv = pv->next) {
      xmlTextWriterWriteFormatElement(writer, BAD_CAST "privilege",
        "%s", BAD_CAST pv->text);
    }
    xmlTextWriterEndElement(writer);
  }

  const auto& accounts =
      context_->manifest_plugins_data.get().account_info.get().accounts();
  if (!accounts.empty()) {
    xmlTextWriterStartElement(writer, BAD_CAST "account");
    // add account info
    for (auto& account : accounts) {
      xmlTextWriterStartElement(writer, BAD_CAST "account-provider");

      xmlTextWriterWriteAttribute(writer, BAD_CAST "appid",
                                  BAD_CAST account.appid.c_str());

      if (account.multiple_account_support)
        xmlTextWriterWriteAttribute(writer,
                                    BAD_CAST "multiple-accounts-support",
                                    BAD_CAST "true");
      for (auto& icon_pair : account.icon_paths) {
        xmlTextWriterStartElement(writer, BAD_CAST "icon");
        if (icon_pair.first == "AccountSmall")
          xmlTextWriterWriteAttribute(writer, BAD_CAST "section",
                                      BAD_CAST "account-small");
        else
          xmlTextWriterWriteAttribute(writer, BAD_CAST "section",
                                      BAD_CAST "account");
        xmlTextWriterWriteString(writer, BAD_CAST icon_pair.second.c_str());
        xmlTextWriterEndElement(writer);
      }

      for (auto& name_pair : account.names) {
        xmlTextWriterStartElement(writer, BAD_CAST "label");
        if (!name_pair.first.empty())
          xmlTextWriterWriteAttribute(writer, BAD_CAST "xml:lang",
                                      BAD_CAST name_pair.second.c_str());
        xmlTextWriterWriteString(writer, BAD_CAST name_pair.first.c_str());
        xmlTextWriterEndElement(writer);
      }

      for (auto& capability : account.capabilities) {
        xmlTextWriterWriteFormatElement(writer, BAD_CAST "capability",
          "%s", BAD_CAST capability.c_str());
      }

      xmlTextWriterEndElement(writer);
    }
    xmlTextWriterEndElement(writer);
  }

  xmlTextWriterEndElement(writer);

  xmlTextWriterEndDocument(writer);
  xmlFreeTextWriter(writer);

  if (pkgmgr_parser_check_manifest_validation(
      context_->xml_path.get().c_str()) != 0) {
    LOG(ERROR) << "Manifest is not valid";
    return Step::Status::ERROR;
  }

  LOG(DEBUG) << "Successfully create manifest xml "
      << context_->xml_path.get();
  return Status::OK;
}

common_installer::Step::Status StepGenerateXml::undo() {
  bs::error_code error;
  if (fs::exists(context_->xml_path.get()))
    fs::remove_all(context_->xml_path.get(), error);
  return Status::OK;
}

}  // namespace pkgmgr
}  // namespace common_installer
