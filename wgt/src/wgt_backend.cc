/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include <pkgmgr_installer.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <cerrno>
#include <cassert>
#include <memory>
#include <app_installer.h>
#include <step_unzip.h>
#include "step/signature_step.h"

int
main (int argc, char **argv)
{
  int result;
  AppInstaller *Installer = NULL;
  step_unzip *step_unpack = NULL;

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
  switch (pkgmgr_installer_get_request_type (pi)) {
    case PKGMGR_REQ_INSTALL: {
    Installer = new AppInstaller(PKGMGR_REQ_INSTALL, pkgmgr_installer_get_request_info(pi), "");
    step_unpack = new step_unzip();
    
    Installer->AddStep(step_unpack);

    // FIXME: unique_ptr because steps are not freed in installer.
    std::unique_ptr<common::SignatureStep> signature_step(
         new common::SignatureStep);
    Installer->AddStep(signature_step.get());

    Installer->Run();
      break;
    }
    case PKGMGR_REQ_UNINSTALL: {
      break;
    }
    default: {
      /* unsupported operation */
      result = EINVAL;
      break;
    }
  }
  pkgmgr_installer_free (pi);
  return result;
}
