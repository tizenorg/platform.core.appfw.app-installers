#include "Manifest.h"
#include <iostream>
#include <libxml/xmlreader.h>

using namespace std;

namespace tpk {
namespace ManifestParser {


/* process a node
 */
static void
Manifest::processNode(xmlTextReaderPtr reader) {
  const xmlChar *name, *value;
  xmlReaderTypes type;
  int isEmpty, hasValue

  // Get the current node info
  name = xmlTextReaderConstName(reader);
  if (name == NULL) {
    name = BAD_CAST "--";
  }
  value = xmlTextReaderConstValue(reader);
  type = (xmlReaderTypes) xmlTextReaderNodeType(reader);
  isEmpty = xmlTextReaderIsEmptyElement(reader);
  hasValue = xmlTextReaderHasValue(reader);

}

static void
Manifest::streamFile(const char* filePath) {
  xmlTextReaderPtr reader;
  int ret;

  reader = xmlReaderForFile(filePath, NULL, 0);	// TODO: add DTD validation
  if(reader == NULL) {
    // TODO: File open failure
    throw new FileOpenFaliureException("File open failure");
  } else {
    ret = xmlTextReaderRead(reader);
    while(ret == 1) {
      processNode(reader);
      ret = xmlTextReaderRead(reader);
    }
    xmlFreeTextReader(reader);
    if(ret != 0) {
      // TODO: parse failure
      throw new ParseFailureException("XML parse failure");
    }
  }
}


/* constructor
 */
Manifest::Manifest(const char* filePath) {
  streamFile(filePath);
}

/* destructor
 */
Manifest::~Manifest() {
}


}   // ns: ManifestParser
}   // ns: tpk
