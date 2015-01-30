/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */

#include "tpk/ManifestParser.h"
#include <libxml/xmlreader.h>
#include <iostream>
#include <vector>
#include "tpk/XmlNodes.h"

namespace tpk {


XmlNodeManifest& ManifestParser::getManifest() {
  return manifest;
}


template <typename T>
void ManifestParser::checkAndSetNode(xmlTextReaderPtr reader,
    const char* name,
    const T &xmlNode) {
  T& node = const_cast<T &>(xmlNode);
  if (xmlStrEqual((const xmlChar *)name, xmlTextReaderConstName(reader)) &&
      xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
    node.init(reader);
  }
}


template <typename T>
void ManifestParser::checkAndAppendNode(xmlTextReaderPtr reader,
    const char* name,
    const std::vector<T*> &v) {
  if (xmlStrEqual((const xmlChar *)name, xmlTextReaderConstName(reader)) &&
      xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
    T *p = new T();
    p->init(reader);
    std::vector<T*>& _v = const_cast<std::vector<T*>&>(v);
    _v.push_back(p);
  }
}


void ManifestParser::processNode(xmlTextReaderPtr reader) {
  // For each node, find required node names and get data
  checkAndSetNode(reader, "manifest", manifest);
  checkAndSetNode(reader, "ui-application", manifest.ui_application);
  checkAndSetNode(reader, "icon", manifest.ui_application.icon);
  checkAndSetNode(reader, "label", manifest.ui_application.label);
  checkAndSetNode(reader, "privileges", manifest.privileges);
  checkAndAppendNode(reader, "privilege", manifest.privileges.v_privilege);
}


void ManifestParser::streamFile(const char* filePath) {
  xmlTextReaderPtr reader;
  int ret;

  // TODO(youmin.ha@samsung.com): add DTD validation
  reader = xmlReaderForFile(filePath, NULL, 0);
  if (reader == NULL) {
    throw new FileOpenFailureException();
  } else {
    ret = xmlTextReaderRead(reader);
    while (ret == 1) {
      processNode(reader);
      ret = xmlTextReaderRead(reader);
    }
    xmlFreeTextReader(reader);
    if (ret != 0) {
      throw new ParseFailureException();
    }
  }
}


/* constructor
 */
ManifestParser::ManifestParser(const char* filePath) {
  LIBXML_TEST_VERSION

  streamFile(filePath);

  xmlCleanupParser();
  xmlMemoryDump();
}


/* destructor
 */
ManifestParser::~ManifestParser() {
}


}  // namespace tpk
