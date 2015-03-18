/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#include "tpk/task.h"
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
#include "tpk/step/step_parse.h"
#include "tpk/step/step_symbolic_link.h"
#include "utils/logging.h"
#endif


namespace ci = common_installer;

namespace {
  const char kPkgType[] = "tpk";
}  // namespace

namespace tpk {

SCOPE_LOG_TAG(TpkTask)

/* Constructor
 */
Task::Task() :
  pi_(nullptr),
  request_(PKGMGR_REQ_INVALID) {
}


/* Destructor
 */
::tpk::Task::~Task() {
  if (pi_) {
    pkgmgr_installer_free(pi_);
    pi_ = nullptr;
  }
}


bool Task::Init(int argc, char** argv) {
  pi_ = pkgmgr_installer_new();
  if (!pi_) {
    LOG(ERROR) << "Failed to run pkgmgr_installer_new()";
    return false;
  }
  if (!!pkgmgr_installer_receive_request(pi_, argc, argv)) {
    LOG(ERROR) << "Invalid argument";
    pkgmgr_installer_free(pi_);
    pi_ = nullptr;
    return false;
  }
  request_ = pkgmgr_installer_get_request_type(pi_);

  return true;
}


bool Task::Run() {
  bool ret = false;
  switch (request_) {
    case PKGMGR_REQ_INSTALL:
      ret = Install();
      break;
    case PKGMGR_REQ_UNINSTALL:
      ret = Uninstall();
      break;
    case PKGMGR_REQ_REINSTALL:
      ret = Reinstall();
      break;
  }
  return ret;
}

bool Task::Install() {
  ci::AppInstaller ai(pi_, kPkgType);

  ai.AddStep<ci::unzip::StepUnzip>();
  ai.AddStep<ci::signature::StepSignature>();
  ai.AddStep<tpk::step::StepParse>();
  ai.AddStep<ci::signal::StepSignal>();
  ai.AddStep<ci::copy::StepCopy>();
  ai.AddStep<tpk::step::StepSymbolicLink>();
  ai.AddStep<ci::security::StepSecurity>();
  ai.AddStep<ci::generate_xml::StepGenerateXml>();
  ai.AddStep<ci::record::StepRecord>();

  return ai.Run();
}

bool Task::Uninstall() {
  ci::AppInstaller ai(pi_, kPkgType);

  ai.AddStep<ci::parse::StepParse>();
  ai.AddStep<ci::signal::StepSignal>();
  ai.AddStep<ci::unregister::StepUnregister>();
  ai.AddStep<ci::remove::StepRemove>();
  ai.AddStep<ci::revoke_security::StepRevokeSecurity>();

  return ai.Run();
}

bool Task::Reinstall() {
  return false;
}

}  // namespace tpk

