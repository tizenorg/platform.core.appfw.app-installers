#include "step_register.h"

#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <climits>
#include <cstring>
#include <string>
#include <sys/stat.h>
#include <cassert>
#include <fcntl.h>

const char* appinst_tags[] = {"removable=true", NULL, };

step_register::step_register()
{}

int step_register::process (Context_installer* data) {
	assert (data->manifest_path != NULL);

	data->manifest = getuid() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER) ?
													pkgmgr_parser_usr_process_manifest_xml(data->manifest_path, getuid()) :
													pkgmgr_parser_process_manifest_xml(data->manifest_path);

	if (data->manifest == NULL) {
		std::cout << "[process register]: Failed to parsing manifest"<< std::endl;
		return APPINST_R_ERROR;
	}

	int ret = getuid() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER) ?
													pkgmgr_parser_parse_usr_manifest_for_installation(data->manifest_path, getuid(), const_cast<char**>(appinst_tags)) :
													pkgmgr_parser_parse_manifest_for_installation(data->manifest_path, const_cast<char**>(appinst_tags));

	if (ret != 0) {
		std::cout << "[process register]: Failed to parse manifest xml"<< std::endl;
		return APPINST_R_ERROR;
	}

	return APPINST_R_OK;
}

int step_register::clean (Context_installer* data) {
	int ret = APPINST_R_OK;

	return ret;
}

int step_register::undo (Context_installer* data) {
	return clean(data);
}
