/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <app_installer.h>
#include <step_unzip.h>
#include <step_copy.h>


int
main (int argc, char **argv)
{
  int result;
  AppInstaller *Installer = NULL;
  step_unzip *stepUnpack = NULL;
  step_copy *stepCopy = NULL;

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
    stepUnpack = new step_unzip();
    stepCopy  = new step_copy();
    Installer->AddStep(stepUnpack);
    Installer->AddStep(stepCopy);

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
