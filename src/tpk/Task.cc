#include "Task.h"
#include "Exception.h"

#ifdef HOSTTEST
#include "test/mock_pkgmgr_installer.h"
#else
#include <pkgmgr_installer.h>
#include "common/app_installer.h"
#include "common/step/step_unzip.h"
#include "common/step/step_signature.h"
#include "Step/StepParse.h"
#endif

static const char* pkgtype = "tpk";

namespace ci = common_installer;

namespace tpk {

  /** Constructor
  */
Task::Task(const int argc, char** argv) {
  pi = pkgmgr_installer_new();
  if(!pi) {
    throw new tpk::Exception("Not enough memory");
  }
  if( !! pkgmgr_installer_receive_request(pi, argc, argv) ) {
    throw new tpk::Exception("Invalid Argument");
  }
  request = pkgmgr_installer_get_request_type(pi);
}

/** Destructor
*/
Task::~Task() {
  if(pi) {
    pkgmgr_installer_free(pi);
    pi = NULL;
  }
}

void
Task::run(void) {
  switch(request) {
    case PKGMGR_REQ_INSTALL:
      install();
      break;
    case PKGMGR_REQ_UNINSTALL:
      uninstall();
      break;
    case PKGMGR_REQ_REINSTALL:
      reinstall();
      break;
    default:
      throw new Exception("Unsupported request");
  }
}

void
Task::install(void) {
  ci::AppInstaller ai(pi, pkgtype);

  ai.AddStep<ci::unzip::StepUnzip>();
  //ai.AddStep<ci::signature::StepSignature>();
  ai.AddStep<tpk::Step::StepParse>();

  ai.Run();
}

void
Task::uninstall(void) {
  ci::AppInstaller ai(pi, pkgtype);
  // TODO: Add steps here

  ai.Run();
}

void
Task::reinstall(void) {
  ci::AppInstaller ai(pi, pkgtype);
  // TODO: Add steps here

  ai.Run();
}


}  // end ns: tpk
