/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_generate_xml.h"

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

#include "common/utils/file_util.h"
#include "common/utils/glist_range.h"

namespace bs = boost::system;
namespace bf = boost::filesystem;

namespace {

void WriteUIApplicationAttributes(
    xmlTextWriterPtr writer, application_x *app) {
  xmlTextWriterWriteAttribute(writer, BAD_CAST "taskmanage",
      BAD_CAST "true");
  if (app->nodisplay)
    xmlTextWriterWriteAttribute(writer, BAD_CAST "nodisplay",
        BAD_CAST app->nodisplay);
  if (app->multiple)
    xmlTextWriterWriteAttribute(writer, BAD_CAST "multiple",
        BAD_CAST app->multiple);
  if (app->launch_mode && strlen(app->launch_mode))
    xmlTextWriterWriteAttribute(writer, BAD_CAST "launch_mode",
        BAD_CAST app->launch_mode);
  if (app->ui_gadget && strlen(app->ui_gadget))
    xmlTextWriterWriteAttribute(writer, BAD_CAST "ui-gadget",
        BAD_CAST app->ui_gadget);
  if (app->submode && strlen(app->submode))
    xmlTextWriterWriteAttribute(writer, BAD_CAST "submode",
        BAD_CAST app->submode);
  if (app->submode_mainid && strlen(app->submode_mainid))
    xmlTextWriterWriteAttribute(writer, BAD_CAST "submode-mainid",
        BAD_CAST app->submode_mainid);
  if (app->indicatordisplay && strlen(app->indicatordisplay))
    xmlTextWriterWriteAttribute(writer, BAD_CAST "indicatordisplay",
        BAD_CAST app->indicatordisplay);
  if (app->portraitimg && strlen(app->portraitimg))
    xmlTextWriterWriteAttribute(writer, BAD_CAST "portrait-effectimage",
        BAD_CAST app->portraitimg);
  if (app->landscapeimg && strlen(app->landscapeimg))
    xmlTextWriterWriteAttribute(writer, BAD_CAST "landscape-effectimage",
        BAD_CAST app->landscapeimg);
  if (app->effectimage_type && strlen(app->effectimage_type))
    xmlTextWriterWriteAttribute(writer, BAD_CAST "effectimage-type",
        BAD_CAST app->effectimage_type);
  if (app->hwacceleration && strlen(app->hwacceleration))
    xmlTextWriterWriteAttribute(writer, BAD_CAST "hwacceleration",
        BAD_CAST app->hwacceleration);
}

void WriteServiceApplicationAttributes(
    xmlTextWriterPtr writer, application_x *app) {
  xmlTextWriterWriteAttribute(writer, BAD_CAST "auto-restart",
      BAD_CAST(app->autorestart ? app->autorestart : "false"));
  xmlTextWriterWriteAttribute(writer, BAD_CAST "on-boot",
      BAD_CAST(app->onboot ? app->onboot : "false"));
  xmlTextWriterWriteAttribute(writer, BAD_CAST "taskmanage",
      BAD_CAST "false");
}

void WriteWidgetApplicationAttributes(
    xmlTextWriterPtr writer, application_x *app) {
  if (app->nodisplay)
    xmlTextWriterWriteAttribute(writer, BAD_CAST "nodisplay",
        BAD_CAST app->nodisplay);
  if (app->multiple)
    xmlTextWriterWriteAttribute(writer, BAD_CAST "multiple",
        BAD_CAST app->multiple);
}

}  // namespace

namespace wgt {
namespace pkgmgr {

common_installer::Step::Status StepGenerateXml::GenerateApplicationCommonXml(
    application_x* app, xmlTextWriterPtr writer, AppCompType type) {
  xmlTextWriterWriteAttribute(writer, BAD_CAST "appid", BAD_CAST app->appid);

  // binary is a symbolic link named <appid> and is located in <pkgid>/<appid>
  bf::path exec_path = context_->pkg_path.get()
      / bf::path("bin") / bf::path(app->appid);
  xmlTextWriterWriteAttribute(writer, BAD_CAST "exec",
                              BAD_CAST exec_path.string().c_str());
  if (app->type)
    xmlTextWriterWriteAttribute(writer, BAD_CAST "type", BAD_CAST app->type);
  else
    xmlTextWriterWriteAttribute(writer, BAD_CAST "type", BAD_CAST "capp");

  if (app->process_pool && strlen(app->process_pool))
    xmlTextWriterWriteAttribute(writer, BAD_CAST "process-pool",
                                BAD_CAST app->process_pool);
  // app-specific attributes
  switch (type) {
  case AppCompType::UIAPP:
    WriteServiceApplicationAttributes(writer, app);
    break;
  case AppCompType::SVCAPP:
    WriteUIApplicationAttributes(writer, app);
    break;
  case AppCompType::WIDGETAPP:
    WriteWidgetApplicationAttributes(writer, app);
    break;
  }

  for (label_x* label : GListRange<label_x*>(app->label)) {
    xmlTextWriterStartElement(writer, BAD_CAST "label");
    if (label->lang && strcmp(DEFAULT_LOCALE, label->lang) != 0) {
      xmlTextWriterWriteAttribute(writer, BAD_CAST "xml:lang",
                                  BAD_CAST label->lang);
    }
    xmlTextWriterWriteString(writer, BAD_CAST label->name);
    xmlTextWriterEndElement(writer);
  }

  // icon is renamed to <appid.png>
  if (app->icon) {
    icon_x* iconx = reinterpret_cast<icon_x*>(app->icon->data);
    bf::path app_icon = context_->pkg_path.get();
    // TODO(t.iwanek): type should not be used here
    if (context_->pkg_type.get() == "wgt") {
      app_icon /= "res/wgt";
    } else {
      app_icon /= "shared/res";
    }
    app_icon /= iconx->text;
    bf::path icon = app->appid;
    if (app_icon.has_extension())
      icon += app_icon.extension();
    else
      icon += bf::path(".png");

    if (bf::exists(app_icon)) {
      xmlTextWriterWriteFormatElement(writer, BAD_CAST "icon",
                                          "%s", BAD_CAST icon.c_str());
    }
  } else {
    // Default icon setting is role of the platform
    LOG(DEBUG) << "Icon was not found in package";
  }

  for (image_x* image : GListRange<image_x*>(app->image)) {
    xmlTextWriterStartElement(writer, BAD_CAST "image");
    xmlTextWriterWriteAttribute(writer, BAD_CAST "name",
        BAD_CAST image->name);
    if (image->lang && strcmp(DEFAULT_LOCALE, image->lang) != 0) {
      xmlTextWriterWriteAttribute(writer, BAD_CAST "xml:lang",

                                  BAD_CAST image->lang);
    }
    if (image->section)
      xmlTextWriterWriteAttribute(writer, BAD_CAST "section",
                                  BAD_CAST image->section);
    xmlTextWriterEndElement(writer);
  }

  for (appcontrol_x* appc : GListRange<appcontrol_x*>(app->appcontrol)) {
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

  for (datacontrol_x* datacontrol :
       GListRange<datacontrol_x*>(app->datacontrol)) {
    xmlTextWriterStartElement(writer, BAD_CAST "datacontrol");
    if (datacontrol->access) {
      xmlTextWriterWriteAttribute(writer, BAD_CAST "access",
          BAD_CAST datacontrol->access);
    }
    if (datacontrol->providerid) {
      xmlTextWriterWriteAttribute(writer, BAD_CAST "providerid",
          BAD_CAST datacontrol->providerid);
    }
    if (datacontrol->type) {
      xmlTextWriterWriteAttribute(writer, BAD_CAST "type",
          BAD_CAST datacontrol->type);
    }
    xmlTextWriterEndElement(writer);
  }

  for (metadata_x* meta : GListRange<metadata_x*>(app->metadata)) {
    xmlTextWriterStartElement(writer, BAD_CAST "metadata");
    xmlTextWriterWriteAttribute(writer, BAD_CAST "key",
        BAD_CAST meta->key);
    if (meta->value)
      xmlTextWriterWriteAttribute(writer, BAD_CAST "value",
          BAD_CAST meta->value);
    xmlTextWriterEndElement(writer);
  }

  for (const char* category : GListRange<char*>(app->category)) {
    xmlTextWriterStartElement(writer, BAD_CAST "category");
    xmlTextWriterWriteAttribute(writer, BAD_CAST "name", BAD_CAST category);
    xmlTextWriterEndElement(writer);
  }

  for (const char* background_category : GListRange<char*>(
      app->background_category)) {
    xmlTextWriterStartElement(writer, BAD_CAST "background-category");
    xmlTextWriterWriteAttribute(writer, BAD_CAST "value",
        BAD_CAST background_category);
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

  if (!context_->manifest_data.get()->application) {
    LOG(ERROR) << "No application in package";
    return Step::Status::INVALID_VALUE;
  }
  // TODO(p.sikorski) check context_->uid.get()

  return Step::Status::OK;
}

common_installer::Step::Status StepGenerateXml::process() {
  bf::path xml_path = bf::path(getUserManifestPath(context_->uid.get()))
      / bf::path(context_->pkgid.get());
  xml_path += ".xml";
  context_->xml_path.set(xml_path.string());

  bs::error_code error;
  if (!bf::exists(xml_path.parent_path(), error)) {
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
  xmlTextWriterWriteAttribute(writer, BAD_CAST "api-version",
      BAD_CAST context_->manifest_data.get()->api_version);
  xmlTextWriterWriteAttribute(writer, BAD_CAST "nodisplay-setting",
      BAD_CAST context_->manifest_data.get()->nodisplay_setting);

  for (label_x* label :
       GListRange<label_x*>(context_->manifest_data.get()->label)) {
    xmlTextWriterStartElement(writer, BAD_CAST "label");
    if (label->lang && strcmp(DEFAULT_LOCALE, label->lang) != 0) {
      xmlTextWriterWriteAttribute(writer, BAD_CAST "xml:lang",
                                  BAD_CAST label->lang);
    }
    xmlTextWriterWriteString(writer, BAD_CAST label->name);
    xmlTextWriterEndElement(writer);
  }

  for (author_x* author :
       GListRange<author_x*>(context_->manifest_data.get()->author)) {
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

  for (description_x* description :
       GListRange<description_x*>(context_->manifest_data.get()->description)) {
    xmlTextWriterStartElement(writer, BAD_CAST "description");
    if (description->lang && strcmp(DEFAULT_LOCALE, description->lang) != 0) {
      xmlTextWriterWriteAttribute(writer, BAD_CAST "xml:lang",
                                  BAD_CAST description->lang);
    }
    xmlTextWriterWriteString(writer, BAD_CAST description->text);
    xmlTextWriterEndElement(writer);
  }

  // add application
  for (application_x* app :
       GListRange<application_x*>(context_->manifest_data.get()->application)) {
    AppCompType type;
    if (strcmp(app->component_type, "uiapp") == 0) {
      type = AppCompType::UIAPP;
      xmlTextWriterStartElement(writer, BAD_CAST "ui-application");
    } else if (strcmp(app->component_type, "svcapp") == 0) {
      type = AppCompType::SVCAPP;
      xmlTextWriterStartElement(writer, BAD_CAST "service-application");
    } else if (strcmp(app->component_type, "widgetapp") == 0) {
      type = AppCompType::WIDGETAPP;
      xmlTextWriterStartElement(writer, BAD_CAST "widget-application");
    } else {
      LOG(ERROR) << "Unknown application component_type";
      xmlFreeTextWriter(writer);
      return Status::ERROR;
    }
    GenerateApplicationCommonXml(app, writer, type);
    xmlTextWriterEndElement(writer);
  }

  // add privilege element
  if (context_->manifest_data.get()->privileges) {
    xmlTextWriterStartElement(writer, BAD_CAST "privileges");
    for (const char* priv :
         GListRange<char*>(context_->manifest_data.get()->privileges)) {
      xmlTextWriterWriteFormatElement(writer, BAD_CAST "privilege",
        "%s", BAD_CAST priv);
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

      if (!account.providerid.empty())
        xmlTextWriterWriteAttribute(writer, BAD_CAST "providerid",
                                    BAD_CAST account.providerid.c_str());

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
        if (!name_pair.second.empty())
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

  for (const char* profile :
       GListRange<char*>(context_->manifest_data.get()->deviceprofile)) {
    xmlTextWriterStartElement(writer, BAD_CAST "profile");
    xmlTextWriterWriteAttribute(writer, BAD_CAST "name",
                                BAD_CAST profile);
    xmlTextWriterEndElement(writer);
  }

  const auto& shortcuts =
      context_->manifest_plugins_data.get().shortcut_info.get();
  if (!shortcuts.empty()) {
    xmlTextWriterStartElement(writer, BAD_CAST "shortcut-list");
    for (auto& shortcut : shortcuts) {
      xmlTextWriterStartElement(writer, BAD_CAST "shortcut");
      if (!shortcut.app_id.empty())
        xmlTextWriterWriteAttribute(writer, BAD_CAST "appid",
                                    BAD_CAST shortcut.app_id.c_str());
      if (!shortcut.app_id.empty())
        xmlTextWriterWriteAttribute(writer, BAD_CAST "extra_data",
                                    BAD_CAST shortcut.extra_data.c_str());
      if (!shortcut.app_id.empty())
        xmlTextWriterWriteAttribute(writer, BAD_CAST "extra_key",
                                    BAD_CAST shortcut.extra_key.c_str());
      if (!shortcut.icon.empty()) {
        xmlTextWriterStartElement(writer, BAD_CAST "icon");
        xmlTextWriterWriteString(writer, BAD_CAST shortcut.icon.c_str());
        xmlTextWriterEndElement(writer);
      }
      for (auto& label : shortcut.labels) {
        xmlTextWriterStartElement(writer, BAD_CAST "label");
        if (!label.first.empty())
          xmlTextWriterWriteAttribute(writer, BAD_CAST "xml:lang",
                                      BAD_CAST label.first.c_str());
        xmlTextWriterWriteString(writer, BAD_CAST label.second.c_str());
        xmlTextWriterEndElement(writer);
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
  if (bf::exists(context_->xml_path.get()))
    bf::remove_all(context_->xml_path.get(), error);
  return Status::OK;
}

}  // namespace pkgmgr
}  // namespace wgt
