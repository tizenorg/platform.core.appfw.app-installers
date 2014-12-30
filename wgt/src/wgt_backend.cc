/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <pkgmgr_installer.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <memory>

#include "include/app_installer.h"
#include "include/step/step_unzip.h"
#include "include/step/signature_step.h"
#include "include/step/step_copy.h"

int main(int argc, char **argv) {
  /* get request data */
  pkgmgr_installer *pi = pkgmgr_installer_new();
  if (!pi)
    return ENOMEM;

  int result = pkgmgr_installer_receive_request(pi, argc, argv);
  if (result) {
    pkgmgr_installer_free(pi);
    return -result;
  }

  /* treat the request */
  switch (pkgmgr_installer_get_request_type(pi)) {
    case PKGMGR_REQ_INSTALL: {
      AppInstaller* Installer = new AppInstaller(PKGMGR_REQ_INSTALL,
          pkgmgr_installer_get_request_info(pi), "");

      StepUnzip* step_unpack = new StepUnzip();
      Installer->AddStep(step_unpack);

      // FIXME: unique_ptr because steps are not freed in installer.
      std::unique_ptr<common::SignatureStep> signature_step(
          new common::SignatureStep);
      Installer->AddStep(signature_step.get());

      StepCopy* step_copy = new StepCopy();
      Installer->AddStep(step_copy);

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
  pkgmgr_installer_free(pi);
  return result;
}
