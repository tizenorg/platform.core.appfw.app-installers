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
#include "common/step/step_copy.h"
#include "common/step/step_generate_xml.h"
#include "common/step/step_parse.h"
#include "common/step/step_register_app.h"
#include "common/step/step_remove_files.h"
#include "common/step/step_revoke_security.h"
#include "common/step/step_register_security.h"
#include "common/step/step_check_signature.h"
#include "common/step/step_unregister_app.h"
#include "common/step/step_unzip.h"
#include "wgt/step/step_parse.h"
#include "wgt/step/step_create_symbolic_link.h"


namespace ci = common_installer;

int main(int argc, char** argv) {
  // get request data
  pkgmgr_installer* pi = pkgmgr_installer_new();
  if (!pi)
    return ENOMEM;

  int result = pkgmgr_installer_receive_request(pi, argc, argv);
  if (result) {
    pkgmgr_installer_free(pi);
    return -result;
  }
  common_installer::AppInstaller installer(pi, "wgt");
  /* treat the request */
  switch (pkgmgr_installer_get_request_type(pi)) {
    case PKGMGR_REQ_INSTALL: {
      installer.AddStep<ci::unzip::StepUnzip>();
      installer.AddStep<ci::signature::StepCheckSignature>();
      installer.AddStep<wgt::parse::StepParse>();
      installer.AddStep<ci::copy::StepCopy>();
      installer.AddStep<wgt::symbolic_link::StepCreateSymbolicLink>();
      installer.AddStep<ci::security::StepRegisterSecurity>();
      installer.AddStep<ci::generate_xml::StepGenerateXml>();
      installer.AddStep<ci::register_app::StepRegisterApplication>();
      break;
    }
    case PKGMGR_REQ_UNINSTALL: {
      installer.AddStep<ci::parse::StepParse>();
      installer.AddStep<ci::unregister_app::StepUnregisterApplication>();
      installer.AddStep<ci::remove::StepRemoveFiles>();
      installer.AddStep<ci::revoke_security::StepRevokeSecurity>();
      break;
    }
    default: {
      // unsupported operation
      pkgmgr_installer_free(pi);
      return EINVAL;
    }
  }

  // run request
  result = installer.Run();

  pkgmgr_installer_free(pi);
  return result;
}
