/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <AppInstaller.hxx>

int
main (int argc, char **argv)
{
  int result;
  AppInstaller *Installer;
  /* get request data */
  pkgmgr_installer *pi = pkgmgr_installer_new ();
  if (!pi)
    return ENOMEM;
  result = pkgmgr_installer_receive_request (pi, argc, argv);
  if (result)
    {
      pkgmgr_installer_free (pi);
      return -result;
    }

  /* treat the request */
  switch (pkgmgr_installer_get_request_type (pi))
    {
    case PKGMGR_REQ_INSTALL:
		Installer = new AppInstaller(PKGMGR_REQ_INSTALL,(char*)pkgmgr_installer_get_request_info(pi),NULL);
        //Installer->AddStep(step_unpack);
       
        Installer->Run();
      break;

    case PKGMGR_REQ_UNINSTALL:
	break;


    default:
      /* unsupported operation */
      result = EINVAL;
      break;
    }
  pkgmgr_installer_free (pi);
  return result;
}
