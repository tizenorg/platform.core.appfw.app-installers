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
#include <string>

#include "utils/file_util.h"

#define PKGMGR_LIST_MOVE_NODE_TO_HEAD(list, node) do {                        \
    if (list) { LISTHEAD(list, node); }                                       \
  } while (0)

namespace fs = boost::filesystem;

namespace common_installer {
namespace generate_xml {

Step::Status StepGenerateXml::process() {
  assert(context_->manifest_data());

  fs::path xml_path = fs::path(getUserManifestPath(context_->uid()))
      / fs::path(context_->pkgid());
  xml_path += ".xml";

  context_->xml_path(xml_path.string());
  boost::system::error_code error;
  if ((!context_->manifest_data()->uiapplication) &&
     (!context_->manifest_data()->serviceapplication)) {
    LOG(ERROR) << "There is neither UI applications nor"
               << "Services applications described!";
    return Step::Status::ERROR;
  }

  const char* path = nullptr;
  if (!(path = tzplatform_mkpath(TZ_SYS_RW_ICONS, "app-installers.png"))) {
    LOG(ERROR) << "Internal error of tzplatform config."
                  "Failed to concatenate path.";
    return Step::Status::ERROR;
  }
  fs::path default_icon(path);

  xmlTextWriterPtr writer;

  writer = xmlNewTextWriterFilename(context_->xml_path().c_str(), 0);
  if (!writer) {
    LOG(ERROR) << "Failed to create new file";
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

  if ( context_->manifest_data()->description &&
      context_->manifest_data()->description->name )
    xmlTextWriterWriteFormatElement(writer, BAD_CAST "description",
        "%s", BAD_CAST context_->manifest_data()->description->name);
  else
    xmlTextWriterWriteFormatElement(writer, BAD_CAST "description",
        "%s", BAD_CAST "");

  // add ui-application element per ui application
  for (uiapplication_x* ui = context_->manifest_data()->uiapplication;
      ui != nullptr; ui = ui->next) {
    xmlTextWriterStartElement(writer, BAD_CAST "ui-application");

    xmlTextWriterWriteAttribute(writer, BAD_CAST "appid",
                                      BAD_CAST ui->appid);

    // binary is a symbolic link named <appid> and is located in <pkgid>/<appid>
    fs::path exec_path = fs::path(context_->pkg_path()) / fs::path(ui->appid)
        / fs::path("bin") / fs::path(ui->appid);

    xmlTextWriterWriteAttribute(writer, BAD_CAST "exec",
        BAD_CAST exec_path.string().c_str());
    if ( ui->type )
      xmlTextWriterWriteAttribute(writer, BAD_CAST "type",
          BAD_CAST ui->type);
    else
      xmlTextWriterWriteAttribute(writer, BAD_CAST "type",
          BAD_CAST "capp");
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

    if (ui->icon && ui->icon->name) {
      fs::path app_icon = fs::path(context_->pkg_path()) / fs::path(ui->appid)
        / fs::path(ui->icon->name);
      if (fs::exists(app_icon))
        fs::rename(app_icon, icon_path_ /= icon);
    } else {
      fs::create_symlink(default_icon, icon_path_ /= icon, error);
      LOG(DEBUG) << "Icon is not found in package, the default icon is setting";
    }

    xmlTextWriterWriteFormatElement(writer, BAD_CAST "icon",
                                         "%s", BAD_CAST icon.c_str());

    appcontrol_x* appc_ui = nullptr;
    PKGMGR_LIST_MOVE_NODE_TO_HEAD(ui->appcontrol, appc_ui);
    for (; appc_ui != nullptr; appc_ui = appc_ui->next) {
      xmlTextWriterStartElement(writer, BAD_CAST "app-control");

      if (appc_ui->operation) {
        xmlTextWriterStartElement(writer, BAD_CAST "operation");
        xmlTextWriterWriteAttribute(writer, BAD_CAST "name",
            BAD_CAST appc_ui->operation->name);
        xmlTextWriterEndElement(writer);
      }
      if (appc_ui->uri) {
        xmlTextWriterStartElement(writer, BAD_CAST "uri");
        xmlTextWriterWriteAttribute(writer, BAD_CAST "name",
            BAD_CAST appc_ui->uri->name);
        xmlTextWriterEndElement(writer);
      }
      if (appc_ui->mime) {
        xmlTextWriterStartElement(writer, BAD_CAST "mime");
        xmlTextWriterWriteAttribute(writer, BAD_CAST "name",
            BAD_CAST appc_ui->mime->name);
        xmlTextWriterEndElement(writer);
      }

      xmlTextWriterEndElement(writer);
    }
    xmlTextWriterEndElement(writer);
  }

  // add service-application element per service application
  for (serviceapplication_x* svc =
      context_->manifest_data()->serviceapplication;
      svc != nullptr; svc = svc->next) {
    xmlTextWriterStartElement(writer, BAD_CAST "service-application");

    xmlTextWriterWriteAttribute(writer, BAD_CAST "appid", BAD_CAST svc->appid);

    // binary is a symbolic link named <appid> and is located in <pkgid>/<appid>
    fs::path exec_path = fs::path(context_->pkg_path()) / fs::path(svc->appid)
        / fs::path("bin") / fs::path(svc->appid);

    xmlTextWriterWriteAttribute(writer, BAD_CAST "exec",
        BAD_CAST exec_path.string().c_str());
    xmlTextWriterWriteAttribute(writer, BAD_CAST "type",
        BAD_CAST svc->type);
    if (svc->label) {
      xmlTextWriterWriteFormatElement(writer, BAD_CAST "label",
          "%s", BAD_CAST svc->label->name);
    }

    // the icon is renamed to <appid.png>
    // and located in TZ_USER_ICON/TZ_SYS_ICON
    // if the icon isn't exist print the default icon app-installers.png
    icon_path_ = fs::path(getIconPath(context_->uid()));
    utils::CreateDir(icon_path_);
    fs::path icon = fs::path(svc->appid) += fs::path(".png");

    if (svc->icon && svc->icon->name) {
      fs::path app_icon = fs::path(context_->pkg_path()) / fs::path(svc->appid)
          / fs::path(svc->icon->name);
      if (fs::exists(app_icon))
        fs::rename(app_icon, icon_path_ /= icon);
    } else {
      fs::rename(default_icon, icon_path_ /= icon);
      LOG(DEBUG) << "Icon is not found in package, the default icon is setting";
    }

    appcontrol_x* appc_svc = nullptr;
    PKGMGR_LIST_MOVE_NODE_TO_HEAD(svc->appcontrol, appc_svc);
    for (; appc_svc != nullptr; appc_svc = appc_svc->next) {
      xmlTextWriterStartElement(writer, BAD_CAST "app-control");

      if (appc_svc->operation) {
        xmlTextWriterStartElement(writer, BAD_CAST "operation");
        xmlTextWriterWriteAttribute(writer, BAD_CAST "name",
            BAD_CAST appc_svc->operation->name);
        xmlTextWriterEndElement(writer);
      }
      if (appc_svc->uri) {
        xmlTextWriterStartElement(writer, BAD_CAST "uri");
        xmlTextWriterWriteAttribute(writer, BAD_CAST "name",
            BAD_CAST appc_svc->uri->name);
        xmlTextWriterEndElement(writer);
      }
      if (appc_svc->mime) {
        xmlTextWriterStartElement(writer, BAD_CAST "mime");
        xmlTextWriterWriteAttribute(writer, BAD_CAST "name",
            BAD_CAST appc_svc->mime->name);
        xmlTextWriterEndElement(writer);
      }

      xmlTextWriterEndElement(writer);
    }
    xmlTextWriterEndElement(writer);
  }

  // add privilege element
  privileges_x* pvlg = nullptr;
  PKGMGR_LIST_MOVE_NODE_TO_HEAD(context_->manifest_data()->privileges, pvlg);
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
      context_->xml_path().c_str()) != 0) {
    LOG(ERROR) << "Manifest is not valid";
    return Step::Status::ERROR;
  }

  LOG(DEBUG) << "Successfully create manifest xml " << context_->xml_path();
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
