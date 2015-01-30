#include <iostream>
#include <libxml/xmlreader.h>

#include "Manifest.h"
#include "XmlNodes.h"

using namespace std;

namespace tpk {
namespace ManifestParser {


XmlNodeManifest &
Manifest::getManifest() {
  return manifest;
}

template <typename T>
void
Manifest::checkAndSetNode(xmlTextReaderPtr reader, const char* name, T &xmlNode) {
	if (xmlStrEqual((const xmlChar *)name, xmlTextReaderConstName(reader)) &&
		xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
			xmlNode.init(reader);
	}
}

template <typename T>
void
Manifest::checkAndAppendNode(xmlTextReaderPtr reader, const char* name, std::vector<T*> &v) {
  if (xmlStrEqual((const xmlChar *)name, xmlTextReaderConstName(reader)) &&
      xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
    T *p = new T();
    p->init(reader);
    v.push_back(p);
  }
}


/* process a node
 */
void
Manifest::processNode(xmlTextReaderPtr reader) {
  // For each node, find required node names and get data
  checkAndSetNode(reader, "manifest", manifest);
  checkAndSetNode(reader, "ui-application", manifest.ui_application);
  checkAndSetNode(reader, "icon", manifest.ui_application.icon);
  checkAndSetNode(reader, "label", manifest.ui_application.label);
  checkAndSetNode(reader, "privileges", manifest.privileges);
  checkAndAppendNode(reader, "privilege", manifest.privileges.v_privilege);
}

void
Manifest::streamFile(const char* filePath) {
  xmlTextReaderPtr reader;
  int ret;

  reader = xmlReaderForFile(filePath, NULL, 0);	// TODO: add DTD validation
  if(reader == NULL) {
    // TODO: File open failure
    throw new FileOpenFailureException();
  } else {
    ret = xmlTextReaderRead(reader);
    while(ret == 1) {
      processNode(reader);
      ret = xmlTextReaderRead(reader);
    }
    xmlFreeTextReader(reader);
    if(ret != 0) {
      // TODO: parse failure
      throw new ParseFailureException();
    }
  }
}


/* constructor
 */
Manifest::Manifest(const char* filePath) {
  LIBXML_TEST_VERSION

  streamFile(filePath);

  xmlCleanupParser();
  xmlMemoryDump();
}

/* destructor
 */
Manifest::~Manifest() {
}


}   // ns: ManifestParser
}   // ns: tpk
