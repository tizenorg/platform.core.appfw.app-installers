/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include <iostream>
#include <cstdio>
#include <cstdlib>

#include "common/app_installer.h"
#include "common/step.h"

namespace {

API manifest_x *pkgmgr_parser_process_manifest_xml(const char *manifest) {
  _LOGD("parsing start pkgmgr_parser_process_manifest_xml\n");
  xmlTextReaderPtr reader;
  manifest_x *mfx = NULL;

  reader = xmlReaderForFile(manifest, NULL, 0);
  if (reader) {
    mfx = malloc(sizeof(manifest_x));
    if (mfx) {
      memset(mfx, '\0', sizeof(manifest_x));
      if (__process_manifest(reader, mfx, GLOBAL_USER) < 0) {
        _LOGD("Parsing Failed\n");
        pkgmgr_parser_free_manifest_xml(mfx);
        mfx = NULL;
      } else {
        _LOGD("Parsing Success\n");
      }
    } else {
      _LOGD("Memory allocation error\n");
    }
    xmlFreeTextReader(reader);
  } else {
    _LOGD("Unable to create xml reader\n");
  }
  return mfx;
}

}  // namespace

namespace wgt {
namespace parse {

Class ConfigFileParser {
ConfigFileParser(char * file) {
}
}

class Step {
// process

// clean

// undo
};

}  // namespace parse
}  // namespace wgt
