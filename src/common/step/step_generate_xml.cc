/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "common/step/step_generate_xml.h"

#include <libxml/parser.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <pkgmgr-info.h>
#include <pkgmgr_parser.h>
#include <tzplatform_config.h>
#include <unistd.h>

#include <cassert>
#include <cstring>
#include <iostream>
#include <string>

#include "common/utils.h"

#define DBG(msg) std::cout << "[GenerateXML] " << msg << std::endl;
#define ERR(msg) std::cout << "[ERROR: GenerateXML] " << msg << std::endl;

namespace common_installer {
namespace generate_xml {

fs::path kLauncher ("/usr/lib/xwalk/xwalk");

int StepGenerateXml::process(ContextInstaller* data) {
  assert(data->manifest_data());

  fs::path xml_path = fs::path(getUserManifestPath(data->uid()))
      / fs::path(data->pkgid());
  xml_path.replace_extension(".xml");

  data->set_xml_path(xml_path.string());
  boost::system::error_code error;
  uiapplication_x* ui = data->manifest_data()->uiapplication;
  serviceapplication_x* svc = data->manifest_data()->serviceapplication;

  fs::path default_icon (
      tzplatform_mkpath(TZ_SYS_RW_ICONS, "app-installers.png"));

  xmlTextWriterPtr writer;

  writer = xmlNewTextWriterFilename(data->xml_path().c_str(), 0);
  if (writer == NULL) {
    ERR("Failed to create new file\n");
    return APPINST_R_ERROR;
  }

  xmlTextWriterStartDocument(writer, NULL, NULL, NULL);

  xmlTextWriterSetIndent(writer, 1);

  xmlTextWriterStartElement(writer, BAD_CAST "manifest");

  xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns",
      BAD_CAST "http://tizen.org/ns/packages");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "package",
      BAD_CAST data->manifest_data()->package);
  xmlTextWriterWriteAttribute(writer, BAD_CAST "type",
      BAD_CAST data->manifest_data()->type);
  xmlTextWriterWriteAttribute(writer, BAD_CAST "version",
      BAD_CAST data->manifest_data()->version);

  xmlTextWriterWriteFormatElement(writer, BAD_CAST "label",
      "%s", BAD_CAST data->manifest_data()->label->text);

  xmlTextWriterWriteFormatElement(writer, BAD_CAST "description",
      "%s", BAD_CAST data->manifest_data()->description->name);

  for (ui; ui != NULL; ui = ui->next) {
    xmlTextWriterStartElement(writer, BAD_CAST "ui-application");

    xmlTextWriterWriteAttribute(writer, BAD_CAST "appid",
                                      BAD_CAST ui->appid);

    // binary is a symbolic link named <appid> and is located in <pkgid>/<appid> 
    fs::path exec_path = fs::path(data->pkg_path()) / fs::path(ui->appid)
        / fs::path("bin");
    utils::CreateDir(exec_path);

    exec_path /= fs::path(ui->appid);

    fs::create_symlink(kLauncher, exec_path, error);
    if (error) {
      ERR("Failed to set symbolic link "
        << boost::system::system_error(error).what());
        xmlFreeTextWriter(writer);
      return APPINST_R_ERROR;
    }
    xmlTextWriterWriteAttribute(writer, BAD_CAST "exec",
        BAD_CAST exec_path.string().c_str());
    xmlTextWriterWriteAttribute(writer, BAD_CAST "type",
        BAD_CAST ui->type);
    xmlTextWriterWriteAttribute(writer, BAD_CAST "taskmanage",
        BAD_CAST "true");

    xmlTextWriterWriteFormatElement(writer, BAD_CAST "label",
        "%s", BAD_CAST ui->label->name);

    // the icon is renamed to <appid.png> and located in TZ_USER_ICON/TZ_SYS_ICON
    // if the icon isn't exist print the default icon app-installers.png
    icon_path_ = fs::path(getIconPath(data->uid()));
    utils::CreateDir(icon_path_);
    fs::path icon = fs::path(ui->appid) += fs::path(".png");

    fs::path app_icon = fs::path(data->pkg_path()) / fs::path(ui->appid)
        / fs::path(ui->icon->name);
    if (fs::exists(app_icon)) {
      fs::rename(app_icon, icon_path_ /= icon);
    }
    else {
        fs::create_symlink(default_icon, icon_path_ /= icon, error);
        DBG("Icon is not found in package, the default icon is setting");
    }

    xmlTextWriterWriteFormatElement(writer, BAD_CAST "icon",
                                         "%s", BAD_CAST icon.c_str());

    for (appcontrol_x* appc = ui->appcontrol; appc != NULL; appc = appc->next) {
      xmlTextWriterStartElement(writer, BAD_CAST "app-control");

      xmlTextWriterStartElement(writer, BAD_CAST "operation");
      xmlTextWriterWriteAttribute(writer, BAD_CAST "name",
          BAD_CAST appc->operation->name);

      xmlTextWriterEndElement(writer);

      xmlTextWriterStartElement(writer, BAD_CAST "uri");
      xmlTextWriterWriteAttribute(writer, BAD_CAST "name",
                                      BAD_CAST appc->uri->name);

      xmlTextWriterEndElement(writer);

      xmlTextWriterStartElement(writer, BAD_CAST "mime");
      xmlTextWriterWriteAttribute(writer, BAD_CAST "name",
          BAD_CAST appc->mime->name);

      xmlTextWriterEndElement(writer);

      if (appc->next == NULL)
        break;

      xmlTextWriterEndElement(writer);
    }

    if (ui->next == NULL)
      break;
    xmlTextWriterEndElement(writer);
  }

  for (svc; svc != NULL; svc = svc->next) {
    xmlTextWriterStartElement(writer, BAD_CAST "service-application");

    xmlTextWriterWriteAttribute(writer, BAD_CAST "appid",
                                      BAD_CAST svc->appid);

     // binary is a symbolic link named <appid> and is located in <pkgid>/<appid> 
    fs::path exec_path = fs::path(data->pkg_path()) / fs::path(svc->appid)
        / fs::path("bin");
    utils::CreateDir(exec_path);

    exec_path /= fs::path(svc->appid);

    fs::create_symlink(kLauncher, exec_path, error);
    if (error) {
      ERR("Failed to set symbolic link "
        << boost::system::system_error(error).what());
        xmlFreeTextWriter(writer);
      return APPINST_R_ERROR;
    }

    xmlTextWriterWriteAttribute(writer, BAD_CAST "exec",
        BAD_CAST exec_path.string().c_str());
    xmlTextWriterWriteAttribute(writer, BAD_CAST "type",
        BAD_CAST svc->type);

    xmlTextWriterWriteFormatElement(writer, BAD_CAST "label",
        "%s", BAD_CAST svc->label->name);

    // the icon is renamed to <appid.png> and located in TZ_USER_ICON/TZ_SYS_ICON
    // if the icon isn't exist print the default icon app-installers.png
    icon_path_ = fs::path(getIconPath(data->uid()));
    utils::CreateDir(icon_path_);
    fs::path icon = fs::path(ui->appid) += fs::path(".png");

    fs::path app_icon = fs::path(data->pkg_path()) / fs::path(ui->appid)
        / fs::path(ui->icon->name);
    if (fs::exists(app_icon)) {
      fs::rename(app_icon, icon_path_ /= icon);
    }
    else {
        fs::rename(default_icon, icon_path_ /= icon);
        DBG("Icon is not found in package, the default icon is setting");
    }

    for (appcontrol_x* appc = svc->appcontrol;
        appc != NULL; appc = appc->next) {
      xmlTextWriterStartElement(writer, BAD_CAST "app-control");

      xmlTextWriterStartElement(writer, BAD_CAST "operation");
      xmlTextWriterWriteAttribute(writer, BAD_CAST "name",
          BAD_CAST appc->operation->name);

      xmlTextWriterEndElement(writer);

      xmlTextWriterStartElement(writer, BAD_CAST "uri");
      xmlTextWriterWriteAttribute(writer, BAD_CAST "name",
                                      BAD_CAST appc->uri->name);

      xmlTextWriterEndElement(writer);

      xmlTextWriterStartElement(writer, BAD_CAST "mime");
      xmlTextWriterWriteAttribute(writer, BAD_CAST "name",
          BAD_CAST appc->mime->name);

      xmlTextWriterEndElement(writer);

      if (appc->next == NULL)
        break;

      xmlTextWriterEndElement(writer);
    }

    if (svc->next == NULL)
      break;
    xmlTextWriterEndElement(writer);
  }

  xmlTextWriterEndElement(writer);

  xmlTextWriterEndDocument(writer);

  if (writer == NULL)
    return APPINST_R_ERROR;

  xmlFreeTextWriter(writer);

  if (pkgmgr_parser_check_manifest_validation(data->xml_path().c_str()) != 0) {
      DBG("manifest is not valid");
      return APPINST_R_ERROR;
  }

  DBG("Successfully create manifest xml " << data->xml_path());
  return APPINST_R_OK;
}

int StepGenerateXml::clean(ContextInstaller* data) {
  return APPINST_R_OK;
}

int StepGenerateXml::undo(ContextInstaller* data) {
  if (fs::exists(data->xml_path()))
    fs::remove_all(data->xml_path());

  if (fs::exists(icon_path_))
    fs::remove_all(icon_path_);

  return APPINST_R_OK;
}

}  // namespace generate_xml
}  // namespace common_installer
