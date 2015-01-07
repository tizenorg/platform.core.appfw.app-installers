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

namespace {

const char kLauncher[] = "/usr/lib/xwalk/xwalk";

}  // anonymous namespace

namespace common_installer {
namespace generate_xml {

Step::Status StepGenerateXml::process() {
  assert(context_->manifest_data());

  fs::path xml_path = fs::path(getUserManifestPath(context_->uid()))
      / fs::path(context_->pkgid());
  xml_path.replace_extension(".xml");

  context_->set_xml_path(xml_path.string());
  boost::system::error_code error;
  uiapplication_x* ui = context_->manifest_data()->uiapplication;
  serviceapplication_x* svc = context_->manifest_data()->serviceapplication;
  if ((!ui) && (!svc)) {
    ERR("There is neither UI applications nor"
        << "Services applications described!\n");
    return Step::Status::ERROR;
  }
  appcontrol_x* appc_ui = nullptr;
  appcontrol_x* appc_svc = nullptr;
  if (ui)
    appc_ui = ui->appcontrol;
  if (svc)
    appc_svc = svc->appcontrol;

  privileges_x* pvlg =  context_->manifest_data()->privileges;

  fs::path default_icon(
      tzplatform_mkpath(TZ_SYS_RW_ICONS, "app-installers.png"));

  xmlTextWriterPtr writer;

  writer = xmlNewTextWriterFilename(context_->xml_path().c_str(), 0);
  if (!writer) {
    ERR("Failed to create new file\n");
    return Step::Status::ERROR;
  }

  xmlTextWriterStartDocument(writer, NULL, NULL, NULL);

  xmlTextWriterSetIndent(writer, 1);

  // add manifest Element
  xmlTextWriterStartElement(writer, BAD_CAST "manifest");

  xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns",
      BAD_CAST "http://tizen.org/ns/packages");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "package",
      BAD_CAST context_->manifest_data()->package);
  xmlTextWriterWriteAttribute(writer, BAD_CAST "type",
      BAD_CAST context_->manifest_data()->type);
  xmlTextWriterWriteAttribute(writer, BAD_CAST "version",
      BAD_CAST context_->manifest_data()->version);

  xmlTextWriterWriteFormatElement(writer, BAD_CAST "label",
      "%s", BAD_CAST context_->manifest_data()->label->name);

  xmlTextWriterWriteFormatElement(writer, BAD_CAST "description",
      "%s", BAD_CAST context_->manifest_data()->description->name);

  // add ui-application element per ui application
  for (; ui != nullptr; ui = ui->next) {
    xmlTextWriterStartElement(writer, BAD_CAST "ui-application");

    xmlTextWriterWriteAttribute(writer, BAD_CAST "appid",
                                      BAD_CAST ui->appid);

    // binary is a symbolic link named <appid> and is located in <pkgid>/<appid>
    fs::path exec_path = fs::path(context_->pkg_path()) / fs::path(ui->appid)
        / fs::path("bin");
    utils::CreateDir(exec_path);

    exec_path /= fs::path(ui->appid);

    fs::create_symlink(fs::path(kLauncher), exec_path, error);
    if (error) {
      ERR("Failed to set symbolic link "
        << boost::system::system_error(error).what());
        xmlFreeTextWriter(writer);
      return Step::Status::ERROR;
    }
    xmlTextWriterWriteAttribute(writer, BAD_CAST "exec",
        BAD_CAST exec_path.string().c_str());
    xmlTextWriterWriteAttribute(writer, BAD_CAST "type",
        BAD_CAST "webapp");
    xmlTextWriterWriteAttribute(writer, BAD_CAST "taskmanage",
        BAD_CAST "true");

    xmlTextWriterWriteFormatElement(writer, BAD_CAST "label",
        "%s", BAD_CAST ui->label->name);

    // the icon is renamed to <appid.png>
    // and located in TZ_USER_ICON/TZ_SYS_ICON
    // if the icon isn't exist print the default icon app-installers.png
    icon_path_ = fs::path(getIconPath(context_->uid()));
    utils::CreateDir(icon_path_);
    fs::path icon = fs::path(ui->appid) += fs::path(".png");

    fs::path app_icon = fs::path(context_->pkg_path()) / fs::path(ui->appid)
        / fs::path(ui->icon->name);
    if (fs::exists(app_icon)) {
      fs::rename(app_icon, icon_path_ /= icon);
    } else {
      fs::create_symlink(default_icon, icon_path_ /= icon, error);
      DBG("Icon is not found in package, the default icon is setting");
    }

    xmlTextWriterWriteFormatElement(writer, BAD_CAST "icon",
                                         "%s", BAD_CAST icon.c_str());

    for (; appc_ui != nullptr; appc_ui = appc_ui->next) {
      xmlTextWriterStartElement(writer, BAD_CAST "app-control");

      xmlTextWriterStartElement(writer, BAD_CAST "operation");
      xmlTextWriterWriteAttribute(writer, BAD_CAST "name",
          BAD_CAST appc_ui->operation->name);

      xmlTextWriterEndElement(writer);

      xmlTextWriterStartElement(writer, BAD_CAST "uri");
      xmlTextWriterWriteAttribute(writer, BAD_CAST "name",
                                      BAD_CAST appc_ui->uri->name);

      xmlTextWriterEndElement(writer);

      xmlTextWriterStartElement(writer, BAD_CAST "mime");
      xmlTextWriterWriteAttribute(writer, BAD_CAST "name",
          BAD_CAST appc_ui->mime->name);

      xmlTextWriterEndElement(writer);

      if (!appc_ui->next)
        break;

      xmlTextWriterEndElement(writer);
    }

    if (!ui->next)
      break;
    xmlTextWriterEndElement(writer);
  }

  // add service-application element per service application
  for (; svc != nullptr; svc = svc->next) {
    xmlTextWriterStartElement(writer, BAD_CAST "service-application");

    xmlTextWriterWriteAttribute(writer, BAD_CAST "appid", BAD_CAST svc->appid);

    // binary is a symbolic link named <appid> and is located in <pkgid>/<appid>
    fs::path exec_path = fs::path(context_->pkg_path()) / fs::path(svc->appid)
        / fs::path("bin");
    utils::CreateDir(exec_path);

    exec_path /= fs::path(svc->appid);

    fs::create_symlink(fs::path(kLauncher), exec_path, error);
    if (error) {
      ERR("Failed to set symbolic link "
        << boost::system::system_error(error).what());
        xmlFreeTextWriter(writer);
      return Step::Status::ERROR;
    }

    xmlTextWriterWriteAttribute(writer, BAD_CAST "exec",
        BAD_CAST exec_path.string().c_str());
    xmlTextWriterWriteAttribute(writer, BAD_CAST "type",
        BAD_CAST svc->type);

    xmlTextWriterWriteFormatElement(writer, BAD_CAST "label",
        "%s", BAD_CAST svc->label->name);

    // the icon is renamed to <appid.png>
    // and located in TZ_USER_ICON/TZ_SYS_ICON
    // if the icon isn't exist print the default icon app-installers.png
    icon_path_ = fs::path(getIconPath(context_->uid()));
    utils::CreateDir(icon_path_);
    fs::path icon = fs::path(ui->appid) += fs::path(".png");

    fs::path app_icon = fs::path(context_->pkg_path()) / fs::path(ui->appid)
        / fs::path(ui->icon->name);
    if (fs::exists(app_icon)) {
      fs::rename(app_icon, icon_path_ /= icon);
    } else {
      fs::rename(default_icon, icon_path_ /= icon);
      DBG("Icon is not found in package, the default icon is setting");
    }

    for (; appc_svc != nullptr; appc_svc = appc_svc->next) {
      xmlTextWriterStartElement(writer, BAD_CAST "app-control");

      xmlTextWriterStartElement(writer, BAD_CAST "operation");
      xmlTextWriterWriteAttribute(writer, BAD_CAST "name",
          BAD_CAST appc_svc->operation->name);

      xmlTextWriterEndElement(writer);

      xmlTextWriterStartElement(writer, BAD_CAST "uri");
      xmlTextWriterWriteAttribute(writer, BAD_CAST "name",
                                      BAD_CAST appc_svc->uri->name);

      xmlTextWriterEndElement(writer);

      xmlTextWriterStartElement(writer, BAD_CAST "mime");
      xmlTextWriterWriteAttribute(writer, BAD_CAST "name",
          BAD_CAST appc_svc->mime->name);

      xmlTextWriterEndElement(writer);

      if (!appc_svc->next)
        break;

      xmlTextWriterEndElement(writer);
    }

    if (!svc->next)
      break;
    xmlTextWriterEndElement(writer);
  }

  // add privilege element
  for (; pvlg != nullptr; pvlg = pvlg->next) {
    privilege_x* pv = pvlg->privilege;

    for (; pv != nullptr; pv = pv->next) {
    xmlTextWriterStartElement(writer, BAD_CAST "privilege");
    xmlTextWriterWriteAttribute(writer, BAD_CAST "name",
                                      BAD_CAST pv->text);
      if (!pv->next)
        break;

      xmlTextWriterEndElement(writer);
      }
    if (!pvlg->next)
      break;
  }


  xmlTextWriterEndElement(writer);

  xmlTextWriterEndDocument(writer);

  if (!writer)
    return Step::Status::ERROR;

  xmlFreeTextWriter(writer);

  if (pkgmgr_parser_check_manifest_validation(
      context_->xml_path().c_str()) != 0) {
    DBG("Manifest is not valid");
    return Step::Status::ERROR;
  }

  DBG("Successfully create manifest xml " << context_->xml_path());
  return Status::OK;
}

Step::Status StepGenerateXml::clean() {
  return Status::OK;
}

Step::Status  StepGenerateXml::undo() {
  if (fs::exists(context_->xml_path()))
    fs::remove_all(context_->xml_path());

  if (fs::exists(icon_path_))
    fs::remove_all(icon_path_);

  return Status::OK;
}

}  // namespace generate_xml
}  // namespace common_installer
