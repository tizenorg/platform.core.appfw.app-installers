// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "wgt/wgt_installer.h"

#include <manifest_handlers/widget_config_parser.h>

#include "common/pkgmgr_interface.h"
#include "common/step/step_configure.h"
#include "common/step/step_backup_manifest.h"
#include "common/step/step_backup_icons.h"
#include "common/step/step_copy.h"
#include "common/step/step_copy_backup.h"
#include "common/step/step_copy_storage_directories.h"
#include "common/step/step_fail.h"
#include "common/step/step_generate_xml.h"
#include "common/step/step_open_recovery_file.h"
#include "common/step/step_parse.h"
#include "common/step/step_register_app.h"
#include "common/step/step_recover_application.h"
#include "common/step/step_recover_files.h"
#include "common/step/step_recover_icons.h"
#include "common/step/step_recover_manifest.h"
#include "common/step/step_recover_security.h"
#include "common/step/step_recover_storage_directories.h"
#include "common/step/step_remove_icons.h"
#include "common/step/step_remove_files.h"
#include "common/step/step_remove_temporary_directory.h"
#include "common/step/step_revoke_security.h"
#include "common/step/step_register_security.h"
#include "common/step/step_old_manifest.h"
#include "common/step/step_check_signature.h"
#include "common/step/step_unregister_app.h"
#include "common/step/step_unzip.h"
#include "common/step/step_update_app.h"
#include "common/step/step_update_security.h"
#include "common/step/step_check_old_certificate.h"

#include "wgt/step/step_create_symbolic_link.h"
#include "wgt/step/step_check_settings_level.h"
#include "wgt/step/step_parse.h"
#include "wgt/step/step_parse_recovery.h"
#include "wgt/step/step_remove_encryption_data.h"
#include "wgt/step/step_rds_parse.h"
#include "wgt/step/step_rds_modify.h"
#include "wgt/step/step_wgt_create_icons.h"
#include "wgt/step/step_wgt_create_storage_directories.h"
#include "wgt/step/step_wgt_copy_storage_directories.h"
#include "wgt/step/step_wgt_resource_directory.h"

namespace ci = common_installer;

namespace wgt {

WgtInstaller::WgtInstaller(ci::PkgMgrPtr pkgrmgr)
    : AppInstaller("wgt", pkgrmgr) {
  /* treat the request */
  switch (pkgmgr_->GetRequestType()) {
    case ci::RequestType::Install : {
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::filesystem::StepUnzip>();
      AddStep<wgt::parse::StepParse>(true);
      AddStep<ci::security::StepCheckSignature>();
      AddStep<wgt::security::StepCheckSettingsLevel>();
      AddStep<wgt::filesystem::StepWgtResourceDirectory>();
      AddStep<ci::filesystem::StepCopy>();
      AddStep<wgt::filesystem::StepWgtCreateStorageDirectories>();
      AddStep<wgt::filesystem::StepCreateSymbolicLink>();
      AddStep<wgt::filesystem::StepWgtCreateIcons>();
      AddStep<ci::pkgmgr::StepGenerateXml>();
      AddStep<ci::pkgmgr::StepRegisterApplication>();
      AddStep<ci::security::StepRegisterSecurity>();
      break;
    }
    case ci::RequestType::Update: {
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::filesystem::StepUnzip>();
      AddStep<wgt::parse::StepParse>(true);
      AddStep<ci::security::StepCheckSignature>();
      AddStep<wgt::security::StepCheckSettingsLevel>();
      AddStep<ci::security::StepCheckOldCertificate>();
      AddStep<wgt::filesystem::StepWgtResourceDirectory>();
      AddStep<ci::backup::StepOldManifest>();
      AddStep<ci::backup::StepBackupManifest>();
      AddStep<ci::backup::StepBackupIcons>();
      AddStep<ci::backup::StepCopyBackup>();
      AddStep<wgt::filesystem::StepWgtCopyStorageDirectories>();
      AddStep<wgt::filesystem::StepCreateSymbolicLink>();
      AddStep<wgt::filesystem::StepWgtCreateIcons>();
      AddStep<ci::security::StepUpdateSecurity>();
      AddStep<ci::pkgmgr::StepGenerateXml>();
      AddStep<ci::pkgmgr::StepUpdateApplication>();
      break;
    }
    case ci::RequestType::Uninstall: {
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::parse::StepParse>();
      AddStep<ci::backup::StepBackupManifest>();
      AddStep<ci::pkgmgr::StepUnregisterApplication>();
      AddStep<ci::filesystem::StepRemoveFiles>();
      AddStep<ci::filesystem::StepRemoveIcons>();
      AddStep<wgt::encrypt::StepRemoveEncryptionData>();
      AddStep<ci::security::StepRevokeSecurity>();
      break;
    }
    case ci::RequestType::Reinstall: {
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<wgt::parse::StepParse>(false);
      AddStep<ci::backup::StepOldManifest>();
      AddStep<wgt::rds::StepRDSParse>();
      AddStep<wgt::rds::StepRDSModify>();
      AddStep<ci::security::StepUpdateSecurity>();
      break;
    }
    case ci::RequestType::Recovery: {
      AddStep<ci::configuration::StepConfigure>(pkgmgr_);
      AddStep<ci::recovery::StepOpenRecoveryFile>();
      AddStep<wgt::parse::StepParseRecovery>();
      AddStep<ci::pkgmgr::StepRecoverApplication>();
      AddStep<ci::filesystem::StepRemoveTemporaryDirectory>();
      AddStep<ci::filesystem::StepRecoverIcons>();
      AddStep<ci::filesystem::StepRecoverManifest>();
      AddStep<ci::filesystem::StepRecoverStorageDirectories>();
      AddStep<ci::filesystem::StepRecoverFiles>();
      AddStep<ci::security::StepRecoverSecurity>();
      break;
    }
    default: {
      AddStep<ci::configuration::StepFail>();
    }
  }
}

}  // namespace wgt
