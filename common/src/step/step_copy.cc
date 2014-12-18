#include "step_copy.h"

#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <climits>
#include <cstring>
#include <string>
#include <sys/stat.h>
#include <cassert>
#include <fcntl.h>
#include <tzplatform_config.h>
#include <step_unzip.h>

#define DIR_DELIMITER '/'
#define READ_SIZE 8192
#define DIR_MODE S_IRWXU

enum ELT{
	PKGID,
	APPID
};

step_copy::step_copy()
{}

const char *get_id (const char *manifest_path, ELT elt) {
	char *id = NULL;
	const char *node = NULL;
	const char *attr = NULL;

	if (elt == PKGID) {
	node = strdup("manifest");
	attr = strdup("package");
	} else {
	node = strdup("ui-application");
	attr = strdup("appid");
	}

	xmlTextReaderPtr xml= xmlReaderForFile(manifest_path, NULL, 0);
	if (!xml) {
		std::cout << "[process copy]: Failed to load manifest file." << std::endl;
		xmlFreeTextReader(xml);
		return NULL;
	}
	while (xmlTextReaderRead(xml) == 1) {
		const xmlChar *node_name = xmlTextReaderConstName(xml);
		if (!strcmp((const char *)node_name, node))
		break;
	}
	if (xmlTextReaderGetAttribute(xml, (const xmlChar *)attr))
		asprintf(&id, (const char *)xmlTextReaderGetAttribute(xml, (const xmlChar *)attr));
	else
		std::cout << "[process copy]: Failed to found id in xml" << std::endl;
	xmlFreeTextReader(xml);

	return id;
}

int step_copy::process (Context_installer* data) {
	assert (data->unpack_directory != NULL);
	assert (!access (data->unpack_directory , F_OK));

data->manifest_path = strdup("/home/carol/.applications/manifest/xwalk-service.SM31mV8fq9.go.xml");
	const char* app_path = getuid() != tzplatform_getuid(TZ_SYS_GLOBALAPP_USER) ?
													tzplatform_getenv(TZ_USER_APP) :
													tzplatform_getenv(TZ_SYS_RW_APP);

	const char *appid = get_id(data->manifest_path, APPID);
	if (appid == NULL)
		return APPINST_R_ERROR;
	const char *pkgid = get_id(data->manifest_path, PKGID);
	if (pkgid == NULL)
		return APPINST_R_ERROR;

	char *package_path = NULL;
	asprintf(&package_path, "%s/%s", app_path, pkgid);
	if (access(package_path, F_OK) != 0)
		step_unzip::createDir_u(package_path);
	asprintf(&app_dir, "%s/%s", package_path, appid);

	if (access(app_dir, F_OK) == 0) {
		std::cout << "[process copy] : app directory:  " << app_dir << " already exist!" << std::endl;
		return APPINST_R_ERROR;
	}

	char *config_file = strdup(data->unpack_directory);
	asprintf(&config_file, "config.xml");

	if (rename(data->unpack_directory, app_dir) != 0)
		std::cout << "[process copy] : Failed to move files from "<< data->unpack_directory << " to app dir: " << app_dir << " errno error : "<< errno << std::endl;

	data->pkgid = strdup(get_id(data->manifest_path, PKGID));
	if (data->pkgid == NULL)
		std::cout << "[process copy] : failed to get pkgid "<< std::endl;
	std::cout << "[process copy] : Move files from temp dir" << data->unpack_directory << " into app dir: " << app_dir << std::endl;

	return APPINST_R_OK;
}

int step_copy::clean (Context_installer* data) {

	// Please note that temporary directory is remove in function clean of step unzip
	if (app_dir != NULL)
		free(app_dir);
	std::cout << "[clean copy]" << std::endl;
	return APPINST_R_OK;
}

int step_copy::undo (Context_installer* data) {
	return APPINST_R_OK;
}
