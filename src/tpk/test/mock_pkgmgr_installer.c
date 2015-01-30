#include <stdlib.h>
#include "mock_pkgmgr_installer.h"

struct pkgmgr_installer {
	int none;
};

pkgmgr_installer *
pkgmgr_installer_new() {
	pkgmgr_installer *pi = calloc(1, sizeof(struct pkgmgr_installer));
	return pi;
}

int
pkgmgr_installer_free(pkgmgr_installer* pi) {
	if(pi) free(pi);
	return 0;
}

