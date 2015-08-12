/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#include "tpk/tpk_installer.h"
#include "common/app_installer.h"
#include "common/step/step_configure.h"
#include "common/step/step_backup_icons.h"
#include "common/step/step_backup_manifest.h"
#include "common/step/step_create_icons.h"
#include "common/step/step_create_storage_directories.h"
#include "common/step/step_copy.h"
#include "common/step/step_copy_backup.h"
#include "common/step/step_check_old_certificate.h"
#include "common/step/step_fail.h"
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

namespace ci = common_installer;

namespace {

const char kPkgType[] = "tpk";

}  // namespace

namespace tpk {

TpkInstaller::TpkInstaller(common_installer::PkgMgrPtr pkgmgr)
    : AppInstaller(kPkgType, pkgmgr) {
  Prepare();
}

TpkInstaller::~TpkInstaller() {
}

void TpkInstaller::Prepare() {
  switch (pkgmgr_->GetRequestType()) {
    case ci::RequestType::Install:
      InstallSteps();
      break;
    case ci::RequestType::Update:
      UpdateSteps();
      break;
    case ci::RequestType::Uninstall:
      UninstallSteps();
      break;
    case ci::RequestType::Reinstall:
      ReinstallSteps();
      break;
    case ci::RequestType::Recovery:
      AddStep<ci::configuration::StepFail>();
      // TODO(t.iwanek): recovery mode invocation...
      break;
    default:
      AddStep<ci::configuration::StepFail>();
      break;
  }
}

void TpkInstaller::InstallSteps() {
  AddStep<ci::configuration::StepConfigure>(pkgmgr_);
  AddStep<ci::filesystem::StepUnzip>();
  AddStep<tpk::parse::StepParse>();
  AddStep<ci::security::StepCheckSignature>();
  AddStep<ci::filesystem::StepCopy>();
  AddStep<ci::filesystem::StepCreateStorageDirectories>();
  AddStep<tpk::filesystem::StepCreateSymbolicLink>();
  AddStep<ci::filesystem::StepCreateIcons>();
  AddStep<ci::security::StepRegisterSecurity>();
  AddStep<tpk::filesystem::StepCopyManifestXml>();
  AddStep<ci::pkgmgr::StepRegisterApplication>();
}

void TpkInstaller::UpdateSteps() {
  AddStep<ci::configuration::StepConfigure>(pkgmgr_);
  AddStep<ci::filesystem::StepUnzip>();
  AddStep<tpk::parse::StepParse>();
  AddStep<ci::security::StepCheckSignature>();
  AddStep<ci::security::StepCheckOldCertificate>();
  AddStep<ci::backup::StepOldManifest>();
  AddStep<ci::backup::StepBackupManifest>();
  AddStep<ci::backup::StepBackupIcons>();
  AddStep<ci::backup::StepCopyBackup>();
  AddStep<ci::filesystem::StepCreateStorageDirectories>();
  // TODO(t.iwanek): handle coping storage directories
  AddStep<tpk::filesystem::StepCreateSymbolicLink>();
  AddStep<ci::filesystem::StepCreateIcons>();
  AddStep<ci::security::StepUpdateSecurity>();
  AddStep<tpk::filesystem::StepCopyManifestXml>();
  AddStep<ci::pkgmgr::StepUpdateApplication>();
}

void TpkInstaller::UninstallSteps() {
  AddStep<ci::configuration::StepConfigure>(pkgmgr_);
  AddStep<ci::parse::StepParse>();
  AddStep<ci::backup::StepBackupManifest>();
  AddStep<ci::pkgmgr::StepUnregisterApplication>();
  AddStep<ci::security::StepRevokeSecurity>();
  AddStep<ci::filesystem::StepRemoveFiles>();
  AddStep<ci::filesystem::StepRemoveIcons>();
}

void TpkInstaller::ReinstallSteps() {
  AddStep<ci::configuration::StepFail>();
}

}  // namespace tpk

