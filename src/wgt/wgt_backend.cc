/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include <cerrno>

#include "common/app_installer.h"
#include "common/pkgmgr_interface.h"
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
  int result = ci::PkgMgrInterface::Init(argc, argv);
  if (!result) {
    LOG(ERROR) << "Cannot connect to PkgMgrInstaller";
    return -result;
  }

  ci::PkgMgrPtr pkgmgr = ci::PkgMgrInterface::Instance();

  ci::AppInstaller installer("wgt");
  /* treat the request */
  switch (pkgmgr->GetRequestType()) {
    case ci::PkgMgrInterface::Type::Install : {
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
    case ci::PkgMgrInterface::Type::Uninstall: {
      installer.AddStep<ci::parse::StepParse>();
      installer.AddStep<ci::unregister_app::StepUnregisterApplication>();
      installer.AddStep<ci::remove::StepRemoveFiles>();
      installer.AddStep<ci::revoke_security::StepRevokeSecurity>();
      break;
    }
    default: {
      // unsupported operation
      return EINVAL;
    }
  }

  // run request
  result = installer.Run();

  return result;
}
