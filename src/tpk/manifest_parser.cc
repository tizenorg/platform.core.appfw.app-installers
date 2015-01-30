/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */

#include "tpk/manifest_parser.h"
#include <libxml/xmlreader.h>
#include <iostream>
#include <vector>
#include "tpk/xml_nodes.h"

namespace tpk {


template <typename T>
void ManifestParser::CheckAndSetNode(xmlTextReaderPtr reader,
    const char* name,
    const T& xmlNode) {
  T& node = const_cast<T&>(xmlNode);
  if (xmlStrEqual(reinterpret_cast<const xmlChar*>(name),
                      xmlTextReaderConstName(reader)) &&
          xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
    node.Init(reader);
  }
}


template <typename T>
void ManifestParser::CheckAndAppendNode(xmlTextReaderPtr reader,
    const char* name,
    const std::vector<T*> &v) {
  if (xmlStrEqual(reinterpret_cast<const xmlChar*>(name),
                      xmlTextReaderConstName(reader)) &&
          xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
    T *p = new T();
    p->Init(reader);
    std::vector<T*>& _v = const_cast<std::vector<T*>&>(v);
    _v.push_back(p);
  }
}


void ManifestParser::ProcessNode(xmlTextReaderPtr reader) {
  // For each node, find required node names and get data
  CheckAndSetNode(reader, "manifest", manifest);
  CheckAndSetNode(reader, "ui-application", manifest.ui_application);
  CheckAndSetNode(reader, "icon", manifest.ui_application.icon);
  CheckAndSetNode(reader, "label", manifest.ui_application.label);
  CheckAndSetNode(reader, "privileges", manifest.privileges);
  CheckAndAppendNode(reader, "privilege", manifest.privileges.v_privilege);
}


void ManifestParser::StreamFile(const char* filePath) {
  xmlTextReaderPtr reader;
  int ret;

  // TODO(youmin.ha@samsung.com): add DTD validation
  reader = xmlReaderForFile(filePath, NULL, 0);
  if (reader == nullptr) {
    throw FileOpenFailureException();
  } else {
    ret = xmlTextReaderRead(reader);
    while (ret == 1) {
      ProcessNode(reader);
      ret = xmlTextReaderRead(reader);
    }
    xmlFreeTextReader(reader);
    if (ret != 0) {
      throw ParseFailureException();
    }
  }
}


/* constructor
 */
ManifestParser::ManifestParser(const char* filePath) {
  LIBXML_TEST_VERSION

  StreamFile(filePath);

  xmlCleanupParser();
  xmlMemoryDump();
}


/* destructor
 */
ManifestParser::~ManifestParser() {
}


}  // namespace tpk
