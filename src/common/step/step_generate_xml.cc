/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_generate_xml.h"

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
namespace generate_xml {

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
Step::Status StepGenerateXml::GenerateApplicationCommonXml(T* app,
    xmlTextWriterPtr writer) {
  fs::path default_icon(
      tzplatform_mkpath(TZ_SYS_RW_ICONS, "app-installers.png"));

  // common appributes among uiapplication_x and serviceapplication_x
  xmlTextWriterWriteAttribute(writer, BAD_CAST "appid", BAD_CAST app->appid);

  // binary is a symbolic link named <appid> and is located in <pkgid>/<appid>
  fs::path exec_path = context_->pkg_path.get() / fs::path(app->appid)
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

  // the icon is renamed to <appid.png>
  // and located in TZ_USER_ICON/TZ_SYS_ICON
  // if the icon isn't exist print the default icon app-installers.png
  icon_path_ = fs::path(getIconPath(context_->uid.get()));
  CreateDir(icon_path_);
  fs::path icon = fs::path(app->appid) += fs::path(".png");

  if (app->icon->name) {
    fs::path app_icon = fs::path(context_->pkg_path.get())
      / fs::path(app->appid)
      / fs::path(app->icon->name);
    if (fs::exists(app_icon))
      fs::copy_file(app_icon, icon_path_ /= icon,
                        fs::copy_option::overwrite_if_exists);
  } else {
    boost::system::error_code error;
    fs::create_symlink(default_icon, icon_path_ /= icon, error);
    LOG(DEBUG) << "Icon was not found in package, the default icon will be set";
  }

  xmlTextWriterWriteFormatElement(writer, BAD_CAST "icon",
                                       "%s", BAD_CAST icon.c_str());

  for (appcontrol_x* appc = app->appcontrol; appc != nullptr;
      appc = appc->next) {
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

Step::Status StepGenerateXml::precheck() {
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

Step::Status StepGenerateXml::process() {
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
  for (uiapplication_x* ui = context_->manifest_data.get()->uiapplication;
      ui != nullptr; ui = ui->next) {
    xmlTextWriterStartElement(writer, BAD_CAST "ui-application");
    GenerateApplicationCommonXml(ui, writer);
    xmlTextWriterEndElement(writer);
  }
  // add service-application element per service application
  for (serviceapplication_x* svc =
       context_->manifest_data.get()->serviceapplication;
       svc != nullptr; svc = svc->next) {
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

Step::Status StepGenerateXml::clean() {
  return Status::OK;
}

Step::Status  StepGenerateXml::undo() {
  if (fs::exists(icon_path_))
    fs::remove_all(icon_path_);

  return Status::OK;
}

}  // namespace generate_xml
}  // namespace common_installer
