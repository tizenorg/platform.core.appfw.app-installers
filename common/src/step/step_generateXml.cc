/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "include/step/step_generateXml.h"

#include <libxml/parser.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <pkgmgr-info.h>
#include <pkgmgr_parser.h>
#include <tzplatform_config.h>
#include <unistd.h>

#include <string>
#include <iostream>

#include "include/utils.h"

#define DBG(msg) std::cout << "[GenerateXML] " << msg << std::endl;
#define ERR(msg) std::cout << "[ERROR: GenerateXML] " << msg << std::endl;

namespace common_installer {
namespace generate_xml {

int StepGenerateXml::process(ContextInstaller* data) {
  assert(data->manifest_data());

  uiapplication_x* ui = data->manifest_data()->uiapplication;
  serviceapplication_x* svc = data->manifest_data()->serviceapplication;
  xml_path_ =  fs::path(getUserManifestPath(data->uid()))
      / fs::path(data->pkgid());
  xml_path_.replace_extension(".xml");

  xmlTextWriterPtr writer_;

  writer_ = xmlNewTextWriterFilename(xml_path_.c_str(), 0);
  if (writer_ == NULL) {
    ERR("Failed to create new file\n");
    return APPINST_R_ERROR;
  }

  xmlTextWriterStartDocument(writer_, NULL, NULL, NULL);

  xmlTextWriterSetIndent(writer_, 1);

  xmlTextWriterStartElement(writer_, BAD_CAST "manifest");

  xmlTextWriterWriteAttribute(writer_, BAD_CAST "xmlns",
      BAD_CAST "http://tizen.org/ns/packages");
  xmlTextWriterWriteAttribute(writer_, BAD_CAST "package",
      BAD_CAST data->manifest_data()->package);
  xmlTextWriterWriteAttribute(writer_, BAD_CAST "type",
      BAD_CAST data->manifest_data()->type);
  xmlTextWriterWriteAttribute(writer_, BAD_CAST "version",
      BAD_CAST data->manifest_data()->version);

  xmlTextWriterWriteFormatElement(writer_, BAD_CAST "label",
      "%s", BAD_CAST data->manifest_data()->label->text);

  xmlTextWriterWriteFormatElement(writer_, BAD_CAST "description",
      "%s", BAD_CAST data->manifest_data()->description->name);

  for (ui; ui != NULL; ui = ui->next) {
    xmlTextWriterStartElement(writer_, BAD_CAST "ui-application");

    xmlTextWriterWriteAttribute(writer_, BAD_CAST "appid",
                                      BAD_CAST ui->appid);

    fs::path exec_path = fs::path(data->pkg_path()) / fs::path(ui->appid)
        / fs::path("bin");
    utils::CreateDir(exec_path);

    exec_path /= fs::path(ui->appid);
    if (symlink ("/usr/bin/xwalk-launcher", exec_path.string().c_str()) == -1)
        ERR("Failed to set symbolic link ");

    xmlTextWriterWriteAttribute(writer_, BAD_CAST "exec",
        BAD_CAST exec_path.string().c_str());
    xmlTextWriterWriteAttribute(writer_, BAD_CAST "type",
        BAD_CAST ui->type);
    xmlTextWriterWriteAttribute(writer_, BAD_CAST "taskmanage",
        BAD_CAST "true");

    xmlTextWriterWriteFormatElement(writer_, BAD_CAST "label",
        "%s", BAD_CAST ui->label->name);

    icon_path_ = fs::path(getIconPath(data->uid()));
    utils::CreateDir(icon_path_);
    fs::path icon = fs::path(ui->appid);
    icon.replace_extension(".png");
    rename(fs::path(data->pkg_path()) / fs::path(ui->appid)
        / fs::path(ui->icon->name), icon_path_ /= icon);

    xmlTextWriterWriteFormatElement(writer_, BAD_CAST "icon",
                                         "%s", BAD_CAST icon.string().c_str());

    for (appcontrol_x* appc = ui->appcontrol; appc != NULL; appc = appc->next) {
      xmlTextWriterStartElement(writer_, BAD_CAST "app-control");

      xmlTextWriterStartElement(writer_, BAD_CAST "operation");
      xmlTextWriterWriteAttribute(writer_, BAD_CAST "name",
          BAD_CAST appc->operation->name);

      xmlTextWriterEndElement(writer_);

      xmlTextWriterStartElement(writer_, BAD_CAST "uri");
      xmlTextWriterWriteAttribute(writer_, BAD_CAST "name",
                                      BAD_CAST appc->uri->name);

      xmlTextWriterEndElement(writer_);

      xmlTextWriterStartElement(writer_, BAD_CAST "mime");
      xmlTextWriterWriteAttribute(writer_, BAD_CAST "name",
          BAD_CAST appc->mime->name);

      xmlTextWriterEndElement(writer_);

      if (appc->next == NULL)
        break;

      xmlTextWriterEndElement(writer_);
    }

    if (ui->next == NULL)
      break;
    xmlTextWriterEndElement(writer_);
  }

  for (svc; svc != NULL; svc = svc->next) {
    xmlTextWriterStartElement(writer_, BAD_CAST "service-application");

    xmlTextWriterWriteAttribute(writer_, BAD_CAST "appid",
                                      BAD_CAST svc->appid);

    fs::path exec_path = fs::path(data->pkg_path()) / fs::path(svc->appid)
        / fs::path("bin");
    utils::CreateDir(exec_path);

    exec_path /= fs::path(svc->appid);
    if (symlink ("/usr/bin/xwalk-launcher", exec_path.string().c_str()) == -1)
        ERR("Failed to set symbolic link ");

    xmlTextWriterWriteAttribute(writer_, BAD_CAST "exec",
        BAD_CAST exec_path.string().c_str());
    xmlTextWriterWriteAttribute(writer_, BAD_CAST "type",
        BAD_CAST svc->type);

    xmlTextWriterWriteFormatElement(writer_, BAD_CAST "label",
        "%s", BAD_CAST svc->label->name);

    icon_path_ = fs::path(getIconPath(data->uid()));
    utils::CreateDir(icon_path_);
    fs::path icon = fs::path(svc->appid);
    icon.replace_extension(".png");
    rename(fs::path(data->pkg_path()) / fs::path(svc->appid)
        / fs::path(svc->icon->name), icon_path_ /= icon);

    xmlTextWriterWriteFormatElement(writer_, BAD_CAST "icon",
                                         "%s", BAD_CAST icon.string().c_str());

    for (appcontrol_x* appc = svc->appcontrol;
        appc != NULL; appc = appc->next) {
      xmlTextWriterStartElement(writer_, BAD_CAST "app-control");

      xmlTextWriterStartElement(writer_, BAD_CAST "operation");
      xmlTextWriterWriteAttribute(writer_, BAD_CAST "name",
          BAD_CAST appc->operation->name);

      xmlTextWriterEndElement(writer_);

      xmlTextWriterStartElement(writer_, BAD_CAST "uri");
      xmlTextWriterWriteAttribute(writer_, BAD_CAST "name",
                                      BAD_CAST appc->uri->name);

      xmlTextWriterEndElement(writer_);

      xmlTextWriterStartElement(writer_, BAD_CAST "mime");
      xmlTextWriterWriteAttribute(writer_, BAD_CAST "name",
          BAD_CAST appc->mime->name);

      xmlTextWriterEndElement(writer_);

      if (appc->next == NULL)
        break;

      xmlTextWriterEndElement(writer_);
    }

    if (svc->next == NULL)
      break;
    xmlTextWriterEndElement(writer_);
  }

  xmlTextWriterEndElement(writer_);

  xmlTextWriterEndDocument(writer_);

  if (writer_ == NULL)
    return APPINST_R_ERROR;

  xmlFreeTextWriter(writer_);

  if (!pkgmgr_parser_check_manifest_validation(xml_path_.string().c_str())) {
      DBG("manifest is not valid");
      return APPINST_R_ERROR;
  }

  DBG("Successfully create manifest xml " << xml_path_);
  return APPINST_R_OK;
}

int StepGenerateXml::clean(ContextInstaller* data) {
  return APPINST_R_OK;
}

int StepGenerateXml::undo(ContextInstaller* data) {
  if (fs::exists(xml_path_))
    fs::remove_all(xml_path_);

  if (fs::exists(icon_path_))
    fs::remove_all(icon_path_);

  return APPINST_R_OK;
}

}  // namespace generate_xml
}  // namespace common_installer
