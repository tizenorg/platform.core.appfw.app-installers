#include <stdio.h>
#include <iostream>
#include <libxml/xmlreader.h>

using namespace std;


static void
processNode(xmlTextReaderPtr reader) {
	const xmlChar *name, *value;
	xmlReaderTypes type;

	name = xmlTextReaderConstName(reader);
	if (name == NULL) {
		name = BAD_CAST "--";
	}

	value = xmlTextReaderConstValue(reader);
	type = (xmlReaderTypes) xmlTextReaderNodeType(reader);

	printf("depth(%d) type(%d) %s isEmpty(%d) hasval(%d)",
			xmlTextReaderDepth(reader),
			type,
			name,
			xmlTextReaderIsEmptyElement(reader),
			xmlTextReaderHasValue(reader));
	if (value == NULL) {
		printf("\n");
	} else {
		if (xmlStrlen(value) > 40) {
			printf(" %.40s...\n", value);
		} else {
			printf(" %s\n", value);
		}
	}
}


static void
streamFile(const char* filepath) {
	xmlTextReaderPtr reader;
	int ret;

	reader = xmlReaderForFile(filepath, NULL, 0);
	if (reader != NULL) {
		ret = xmlTextReaderRead(reader);
		while (ret == 1) {
			processNode(reader);
			ret = xmlTextReaderRead(reader);
		}
		xmlFreeTextReader(reader);
		if(ret != 0) {
			cerr << filepath << ": parse failure" << endl;
		}
	} else {
		cerr << "File open failure: " << filepath << endl;
	}
}

int
main(int argc, char* argv[]) {
	if(argc != 2) return 1;

	LIBXML_TEST_VERSION

	streamFile(argv[1]);

	xmlCleanupParser();
	xmlMemoryDump();

	return 0;
}

