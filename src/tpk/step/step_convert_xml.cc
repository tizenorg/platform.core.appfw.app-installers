// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "tpk/step/step_convert_xml.h"

#include <pkgmgr_parser.h>
#include <pkgmgr-info.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/system/error_code.hpp>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

#include <string>

#include "common/utils/glist_range.h"

namespace bs = boost::system;
namespace bf = boost::filesystem;

namespace {

const xmlChar kExecAttributeKey[] = "exec";
const xmlChar kXmlNamespaceUri[] = "http://tizen.org/ns/packages";
const char kXmlXPathAppExpr[] =
    "//*[local-name()='ui-application' or local-name()='service-application' "
    "or local-name()='widget-application']";

}  // namespace

namespace common_installer {
namespace tpk {

Step::Status StepConvertXml::precheck() {
  bf::path xml_path = context_->pkg_path.get();
  xml_path /= "tizen-manifest.xml";

  if (!bf::exists(xml_path)) {
    LOG(ERROR) << "Cannot find manifest file";
    return Step::Status::INVALID_VALUE;
  }

  xml_path_ = xml_path;

  return Step::Status::OK;
}

bool StepConvertXml::ConvertXml(xmlDocPtr doc) {
  xmlXPathContextPtr xpath_ctx;

  xpath_ctx = xmlXPathNewContext(doc);
  if (!xpath_ctx) {
    LOG(ERROR) << "Failed to create XPath context";
    return false;
  }

  for (application_x* app :
       GListRange<application_x*>(context_->manifest_data.get()->application)) {
    std::string expr;
    xmlXPathObjectPtr xpath_obj;
    xmlNodePtr node;

    expr = std::string(kXmlXPathAppExpr) + "[@appid='" + app->appid + "']";
    xpath_obj = xmlXPathEvalExpression((const xmlChar*)expr.c_str(), xpath_ctx);
    if (!xpath_obj || !xpath_obj->nodesetval || !xpath_obj->nodesetval->nodeNr)
      continue;

    node = xpath_obj->nodesetval->nodeTab[0];
    xmlSetProp(node, kExecAttributeKey, (const xmlChar*)app->exec);
    xmlXPathFreeObject(xpath_obj);
  }

  xmlXPathFreeContext(xpath_ctx);

  return true;
}

Step::Status StepConvertXml::process() {
  xmlDocPtr doc = xmlParseFile(xml_path_.string().c_str());
  if (!doc) {
    LOG(ERROR) << "Failed to parse xml file";
    return Step::Status::ERROR;
  }

  if (!ConvertXml(doc))
    return Step::Status::ERROR;

  bf::path new_path = bf::path(getUserManifestPath(context_->uid.get()))
      / bf::path(context_->pkgid.get());
  new_path += ".xml";
  if (xmlSaveFile(new_path.string().c_str(), doc) == -1) {
    LOG(ERROR) << "Failed to write xml file";
    return Step::Status::ERROR;
  }

  context_->xml_path.set(new_path.string());

  if (pkgmgr_parser_check_manifest_validation(
      context_->xml_path.get().c_str()) != 0) {
    LOG(ERROR) << "Manifest is not valid";
    return Step::Status::ERROR;
  }

  LOG(DEBUG) << "Successfully create manifest xml "
      << context_->xml_path.get();

  return Step::Status::OK;
}

Step::Status StepConvertXml::clean() {
  return Step::Status::OK;
}

Step::Status StepConvertXml::undo() {
  bs::error_code error;

  if (bf::exists(context_->xml_path.get()))
    bf::remove_all(context_->xml_path.get(), error);

  return Step::Status::OK;
}

}  // namespace tpk
}  // namespace common_installer
