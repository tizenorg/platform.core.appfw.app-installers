/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#include "tpk/Task.h"
#ifdef HOSTTEST
#include "test/mock_pkgmgr_installer.h"
#else
#include <pkgmgr_installer.h>
#include "common/app_installer.h"
#include "common/step/step_copy.h"
#include "common/step/step_generate_xml.h"
#include "common/step/step_parse.h"
#include "common/step/step_record.h"
#include "common/step/step_remove.h"
#include "common/step/step_revoke_security.h"
#include "common/step/step_security.h"
#include "common/step/step_signal.h"
#include "common/step/step_signature.h"
#include "common/step/step_unregister.h"
#include "common/step/step_unzip.h"
#include "Step/StepParse.h"
#include "Step/StepSymbolicLink.h"
#include "tpk/Exception.h"
#endif


namespace ci = common_installer;
static const char* pkgtype = "tpk";

namespace tpk {

/* Constructor
 */
Task::Task(const int argc, char** argv) {
  pi = pkgmgr_installer_new();
  if (!pi) {
    throw new tpk::Exception("Not enough memory");
  }
  if (!!pkgmgr_installer_receive_request(pi, argc, argv)) {
    throw new tpk::Exception("Invalid Argument");
  }
  request = pkgmgr_installer_get_request_type(pi);
}


/* Destructor
 */
::tpk::Task::~Task() {
  if (pi) {
    pkgmgr_installer_free(pi);
    pi = NULL;
  }
}


void Task::run(void) {
  switch (request) {
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

void Task::install(void) {
  ci::AppInstaller ai(pi, pkgtype);

  ai.AddStep<ci::unzip::StepUnzip>();
  ai.AddStep<ci::signature::StepSignature>();
  ai.AddStep<tpk::Step::StepParse>();
  ai.AddStep<ci::signal::StepSignal>();
  ai.AddStep<ci::copy::StepCopy>();
  ai.AddStep<tpk::Step::StepSymbolicLink>();
  ai.AddStep<ci::security::StepSecurity>();
  ai.AddStep<ci::generate_xml::StepGenerateXml>();
  ai.AddStep<ci::record::StepRecord>();

  ai.Run();
}

void Task::uninstall(void) {
  ci::AppInstaller ai(pi, pkgtype);

  ai.AddStep<ci::parse::StepParse>();
  ai.AddStep<ci::signal::StepSignal>();
  ai.AddStep<ci::revoke_security::StepRevokeSecurity>();
  ai.AddStep<ci::unregister::StepUnregister>();
  ai.AddStep<ci::remove::StepRemove>();

  ai.Run();
}

void Task::reinstall(void) {
}

}  // namespace tpk

