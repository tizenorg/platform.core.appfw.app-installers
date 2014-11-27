#include <include/AppInstaller.hxx>
#include <include/step.hxx>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>




API manifest_x *pkgmgr_parser_process_manifest_xml(const char *manifest)
{
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
			} else
				_LOGD("Parsing Success\n");
		} else {
			_LOGD("Memory allocation error\n");
		}
		xmlFreeTextReader(reader);
	} else {
		_LOGD("Unable to create xml reader\n");
	}
	return mfx;
}


namespace wgt {
namespace step_parse {



Class ConfigFileParser {

ConfigFileParser(char * file)
{
}


}
class Step {
//prcess

//clean

//undo


}
}
