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

#include "common/app_installer.h"
#include "common/step/step_unzip.h"
#include "common/step/step_signature.h"
#include "common/step/step_copy.h"
#include "wgt/step/step_parse.h"

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
      common_installer::AppInstaller* installer =
          new common_installer::AppInstaller(PKGMGR_REQ_INSTALL,
              pkgmgr_installer_get_request_info(pi), "");

      common_installer::unzip::StepUnzip* step_unpack =
          new common_installer::unzip::StepUnzip();
      installer->AddStep(step_unpack);

      // FIXME: unique_ptr because steps are not freed in installer.
      std::unique_ptr<common_installer::signature::StepSignature>
          step_signature(
              new common_installer::signature::StepSignature);
      installer->AddStep(step_signature.get());

      wgt::parse::StepParse* step_parse = new
              wgt::parse::StepParse();
      installer->AddStep(step_parse);

      common_installer::copy::StepCopy* step_copy =
          new common_installer::copy::StepCopy();
      installer->AddStep(step_copy);

      installer->Run();

      delete step_unpack;
      delete step_copy;
      delete step_parse;
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
