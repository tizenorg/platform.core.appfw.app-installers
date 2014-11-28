#ifndef __CTX_INSTALLER_H__
#define __CTX_INSTALLER_H__
#include <unistd.h>
#include <sys/types.h>
#include <pkgmgr_parser.h>
#include <pkgmgr_installer.h>

typedef struct
{
	
	int req; //type of request (Install / Reinstall / Uninstall / Update 
	
	manifest_x * manifest; //Contains the manifest information used to generate xml file
	
	char * pkgid; //pkgid passed in argument in update or uninstallation processing
	
	char * file_path; //file path passed in argument in installtion or Reinstallation process
	
	uid_t uid; // uid of the user that request the operation
	
	char * unpack_directory; // temporary directory path 
	
} Context_installer;

enum {
	APPINST_R_EINVAL = -2,		/**< Invalid argument */
	APPINST_R_ERROR = -1,		/**< General error */
	APPINST_R_OK = 0			/**< General success */
};

#endif
