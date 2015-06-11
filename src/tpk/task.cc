/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#include "tpk/task.h"
#ifdef HOSTTEST
#include "test/mock_pkgmgr_installer.h"
#else
#include "common/app_installer.h"
#include "common/pkgmgr_interface.h"
#include "common/step/step_configure.h"
#include "common/step/step_backup_icons.h"
#include "common/step/step_backup_manifest.h"
#include "common/step/step_copy.h"
#include "common/step/step_copy_backup.h"
#include "common/step/step_copy_storage_directories.h"
#include "common/step/step_create_storage_directories.h"
#include "common/step/step_generate_xml.h"
#include "common/step/step_old_manifest.h"
#include "common/step/step_parse.h"
#include "common/step/step_register_app.h"
#include "common/step/step_remove_files.h"
#include "common/step/step_revoke_security.h"
#include "common/step/step_register_security.h"
#include "common/step/step_check_signature.h"
#include "common/step/step_unregister_app.h"
#include "common/step/step_unzip.h"
#include "common/step/step_update_app.h"
#include "common/step/step_update_security.h"
#include "common/step/step_check_old_certificate.h"
#include "common/utils/logging.h"

#include "tpk/step/step_parse.h"
#include "tpk/step/step_create_symbolic_link.h"
#endif

namespace ci = common_installer;

namespace {

const char kPkgType[] = "tpk";

}  // namespace

namespace tpk {

SCOPE_LOG_TAG(TpkTask)

/* Constructor
 */
Task::Task() {
}


/* Destructor
 */
::tpk::Task::~Task() {
}


bool Task::Init(int argc, char** argv) {
  int result = ci::PkgMgrInterface::Init(argc, argv);
  if (result != 0) {
    LOG(ERROR) << "Cannot connect to PkgMgrInstaller";
    return false;
  }
  return true;
}


bool Task::Run() {
  int ret = 0;
  switch (ci::PkgMgrInterface::Instance()->GetRequestType()) {
    case ci::PkgMgrInterface::Type::Install:
      ret = Install();
      break;
    case ci::PkgMgrInterface::Type::Update:
      ret = Update();
      break;
    case ci::PkgMgrInterface::Type::Uninstall:
      ret = Uninstall();
      break;
    case ci::PkgMgrInterface::Type::Reinstall:
      ret = Reinstall();
      break;
    default:
      break;
  }
  if (ret != 0) {
    LOG(ERROR) << "Got error from AppInstaler: error code " << ret;
    return false;
  }
  return true;
}

int Task::Install() {
  ci::AppInstaller ai(kPkgType);

  ai.AddStep<ci::configure::StepConfigure>();
  ai.AddStep<ci::unzip::StepUnzip>();
  ai.AddStep<tpk::step::StepParse>();
  ai.AddStep<ci::signature::StepCheckSignature>();
  ai.AddStep<ci::copy::StepCopy>();
  ai.AddStep<ci::create_storage::StepCreateStorageDirectories>();
  ai.AddStep<tpk::step::StepCreateSymbolicLink>();
  ai.AddStep<ci::security::StepRegisterSecurity>();
  ai.AddStep<ci::generate_xml::StepGenerateXml>();
  ai.AddStep<ci::register_app::StepRegisterApplication>();

  return ai.Run();
}

int Task::Update() {
  ci::AppInstaller ai(kPkgType);

  ai.AddStep<ci::configure::StepConfigure>();
  ai.AddStep<ci::unzip::StepUnzip>();
  ai.AddStep<tpk::step::StepParse>();
  ai.AddStep<ci::signature::StepCheckSignature>();
  ai.AddStep<ci::old_certificate::StepCheckOldCertificate>();
  ai.AddStep<ci::old_manifest::StepOldManifest>();
  ai.AddStep<ci::backup_manifest::StepBackupManifest>();
  ai.AddStep<ci::backup_icons::StepBackupIcons>();
  ai.AddStep<ci::copy_backup::StepCopyBackup>();
  ai.AddStep<ci::copy_storage::StepCopyStorageDirectories>();
  ai.AddStep<tpk::step::StepCreateSymbolicLink>();
  ai.AddStep<ci::update_security::StepUpdateSecurity>();
  ai.AddStep<ci::generate_xml::StepGenerateXml>();
  ai.AddStep<ci::update_app::StepUpdateApplication>();

  return ai.Run();
}

int Task::Uninstall() {
  ci::AppInstaller ai(kPkgType);

  ai.AddStep<ci::configure::StepConfigure>();
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

