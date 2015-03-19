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
#include "common/step/step_register_app.h"
#include "common/step/step_remove_files.h"
#include "common/step/step_revoke_security.h"
#include "common/step/step_register_security.h"
#include "common/step/step_check_signature.h"
#include "common/step/step_unregister_app.h"
#include "common/step/step_unzip.h"
#include "tpk/step/step_parse.h"
#include "tpk/step/step_create_symbolic_link.h"
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
  int ret = 0;
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
  if (ret != 0) {
    LOG(ERROR) << "Got error from AppInstaler: error code " << ret;
    return false;
  }
  return true;
}

int Task::Install() {
  ci::AppInstaller ai(pi_, kPkgType);

  ai.AddStep<ci::unzip::StepUnzip>();
  ai.AddStep<ci::signature::StepCheckSignature>();
  ai.AddStep<tpk::step::StepParse>();
  ai.AddStep<ci::copy::StepCopy>();
  ai.AddStep<tpk::step::StepCreateSymbolicLink>();
  ai.AddStep<ci::security::StepRegisterSecurity>();
  ai.AddStep<ci::generate_xml::StepGenerateXml>();
  ai.AddStep<ci::register_app::StepRegisterApplication>();

  return ai.Run();
}

int Task::Uninstall() {
  ci::AppInstaller ai(pi_, kPkgType);

  ai.AddStep<ci::parse::StepParse>();
  ai.AddStep<ci::unregister_app::StepUnregisterApplication>();
  ai.AddStep<ci::remove::StepRemoveFiles>();
  ai.AddStep<ci::revoke_security::StepRevokeSecurity>();

  return ai.Run();
}

int Task::Reinstall() {
  return 0;
}

}  // namespace tpk

