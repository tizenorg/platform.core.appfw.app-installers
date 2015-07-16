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
#include "common/step/step_check_old_certificate.h"
#include "common/step/step_generate_xml.h"
#include "common/step/step_old_manifest.h"
#include "common/step/step_parse.h"
#include "common/step/step_register_app.h"
#include "common/step/step_remove_icons.h"
#include "common/step/step_remove_files.h"
#include "common/step/step_revoke_security.h"
#include "common/step/step_register_security.h"
#include "common/step/step_check_signature.h"
#include "common/step/step_unregister_app.h"
#include "common/step/step_unzip.h"
#include "common/step/step_update_app.h"
#include "common/step/step_update_security.h"
#include "common/utils/logging.h"
#include "tpk/step/step_copy_manifest_xml.h"
#include "tpk/step/step_create_symbolic_link.h"
#include "tpk/step/step_parse.h"
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

  ai.AddStep<ci::configuration::StepConfigure>();
  ai.AddStep<ci::filesystem::StepUnzip>();
  ai.AddStep<tpk::parse::StepParse>();
  ai.AddStep<ci::security::StepCheckSignature>();
  ai.AddStep<ci::filesystem::StepCopy>();
  ai.AddStep<ci::filesystem::StepCreateStorageDirectories>();
  ai.AddStep<tpk::filesystem::StepCreateSymbolicLink>();
  ai.AddStep<ci::security::StepRegisterSecurity>();
  ai.AddStep<tpk::filesystem::StepCopyManifestXml>();
  ai.AddStep<ci::pkgmgr::StepRegisterApplication>();

  return ai.Run();
}

int Task::Update() {
  ci::AppInstaller ai(kPkgType);

  ai.AddStep<ci::configuration::StepConfigure>();
  ai.AddStep<ci::filesystem::StepUnzip>();
  ai.AddStep<tpk::parse::StepParse>();
  ai.AddStep<ci::security::StepCheckSignature>();
  ai.AddStep<ci::security::StepCheckOldCertificate>();
  ai.AddStep<ci::backup::StepOldManifest>();
  ai.AddStep<ci::backup::StepBackupManifest>();
  ai.AddStep<ci::backup::StepBackupIcons>();
  ai.AddStep<ci::backup::StepCopyBackup>();
  ai.AddStep<ci::filesystem::StepCopyStorageDirectories>();
  ai.AddStep<tpk::filesystem::StepCreateSymbolicLink>();
  ai.AddStep<ci::security::StepUpdateSecurity>();
  ai.AddStep<tpk::filesystem::StepCopyManifestXml>();
  ai.AddStep<ci::pkgmgr::StepUpdateApplication>();

  return ai.Run();
}

int Task::Uninstall() {
  ci::AppInstaller ai(kPkgType);

  ai.AddStep<ci::configuration::StepConfigure>();
  ai.AddStep<ci::parse::StepParse>();
  ai.AddStep<ci::backup::StepBackupManifest>();
  ai.AddStep<ci::pkgmgr::StepUnregisterApplication>();
  ai.AddStep<ci::filesystem::StepRemoveFiles>();
  ai.AddStep<ci::filesystem::StepRemoveIcons>();
  ai.AddStep<ci::security::StepRevokeSecurity>();

  return ai.Run();
}

int Task::Reinstall() {
  return 0;
}

}  // namespace tpk

