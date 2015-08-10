/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#include "tpk/task.h"
#ifdef HOSTTEST
#include "test/mock_pkgmgr_installer.h"
#else
#include "common/app_installer.h"
#include "common/step/step_configure.h"
#include "common/step/step_backup_icons.h"
#include "common/step/step_backup_manifest.h"
#include "common/step/step_create_icons.h"
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

/* Constructor
 */
Task::Task() {
}


/* Destructor
 */
::tpk::Task::~Task() {
}


bool Task::Init(int argc, char** argv) {
  pkgmgr_ = ci::PkgMgrInterface::Create(argc, argv);
  if (!pkgmgr_) {
    LOG(ERROR) << "Options of pkgmgr installer cannot be parsed";
    return false;
  }
  return true;
}


bool Task::Run() {
  switch (pkgmgr_->GetRequestType()) {
    case ci::RequestType::Install:
      return Install();
    case ci::RequestType::Update:
      return Update();
    case ci::RequestType::Uninstall:
      return Uninstall();
    case ci::RequestType::Reinstall:
      return Reinstall();
    case ci::RequestType::Recovery:
      // TODO(t.iwanek): recovery mode invocation...
      return false;
    default:
      break;
  }
  return false;
}

bool Task::Install() {
  ci::AppInstaller ai(kPkgType, pkgmgr_);

  ai.AddStep<ci::configuration::StepConfigure>(pkgmgr_);
  ai.AddStep<ci::filesystem::StepUnzip>();
  ai.AddStep<tpk::parse::StepParse>();
  ai.AddStep<ci::security::StepCheckSignature>();
  ai.AddStep<ci::filesystem::StepCopy>();
  ai.AddStep<ci::filesystem::StepCreateStorageDirectories>();
  ai.AddStep<tpk::filesystem::StepCreateSymbolicLink>();
  ai.AddStep<ci::filesystem::StepCreateIcons>();
  ai.AddStep<ci::security::StepRegisterSecurity>();
  ai.AddStep<tpk::filesystem::StepCopyManifestXml>();
  ai.AddStep<ci::pkgmgr::StepRegisterApplication>();

  return ai.Run() == ci::AppInstaller::Result::OK;
}

bool Task::Update() {
  ci::AppInstaller ai(kPkgType, pkgmgr_);

  ai.AddStep<ci::configuration::StepConfigure>(pkgmgr_);
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
  ai.AddStep<ci::filesystem::StepCreateIcons>();
  ai.AddStep<ci::security::StepUpdateSecurity>();
  ai.AddStep<tpk::filesystem::StepCopyManifestXml>();
  ai.AddStep<ci::pkgmgr::StepUpdateApplication>();

  return ai.Run() == ci::AppInstaller::Result::OK;
}

bool Task::Uninstall() {
  ci::AppInstaller ai(kPkgType, pkgmgr_);

  ai.AddStep<ci::configuration::StepConfigure>(pkgmgr_);
  ai.AddStep<ci::parse::StepParse>();
  ai.AddStep<ci::backup::StepBackupManifest>();
  ai.AddStep<ci::pkgmgr::StepUnregisterApplication>();
  ai.AddStep<ci::security::StepRevokeSecurity>();
  ai.AddStep<ci::filesystem::StepRemoveFiles>();
  ai.AddStep<ci::filesystem::StepRemoveIcons>();

  return ai.Run() == ci::AppInstaller::Result::OK;
}

bool Task::Reinstall() {
  return false;
}

}  // namespace tpk

