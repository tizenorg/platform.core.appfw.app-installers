/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include <cerrno>

#include "common/app_installer.h"
#include "common/pkgmgr_interface.h"
#include "common/step/step_configure.h"
#include "common/step/step_backup_manifest.h"
#include "common/step/step_backup_icons.h"
#include "common/step/step_copy.h"
#include "common/step/step_copy_backup.h"
#include "common/step/step_copy_storage_directories.h"
#include "common/step/step_generate_xml.h"
#include "common/step/step_parse.h"
#include "common/step/step_register_app.h"
#include "common/step/step_remove_icons.h"
#include "common/step/step_remove_files.h"
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
#include "wgt/step/step_encrypt_resources.h"
#include "wgt/step/step_parse.h"
#include "wgt/step/step_rds_parse.h"
#include "wgt/step/step_rds_modify.h"
#include "wgt/step/step_wgt_create_icons.h"
#include "wgt/step/step_wgt_create_storage_directories.h"
#include "wgt/step/step_wgt_copy_storage_directories.h"
#include "wgt/step/step_wgt_resource_directory.h"
#include "wgt/wgt_app_query_interface.h"

namespace ci = common_installer;

int main(int argc, char** argv) {
  wgt::WgtAppQueryInterface query_interface;
  int result = ci::PkgMgrInterface::Init(argc, argv, &query_interface);
  if (result) {
    LOG(ERROR) << "Cannot connect to PkgMgrInstaller";
    return -result;
  }
  ci::PkgMgrPtr pkgmgr = ci::PkgMgrInterface::Instance();

  ci::AppInstaller installer("wgt");
  /* treat the request */
  switch (pkgmgr->GetRequestType()) {
    case ci::PkgMgrInterface::Type::Install : {
      installer.AddStep<ci::configuration::StepConfigure>();
      installer.AddStep<ci::filesystem::StepUnzip>();
      installer.AddStep<wgt::parse::StepParse>();
      installer.AddStep<ci::security::StepCheckSignature>();
      installer.AddStep<wgt::security::StepCheckSettingsLevel>();
      installer.AddStep<wgt::encrypt::StepEncryptResources>();
      installer.AddStep<wgt::filesystem::StepWgtResourceDirectory>();
      installer.AddStep<ci::filesystem::StepCopy>();
      installer.AddStep<wgt::filesystem::StepWgtCreateStorageDirectories>();
      installer.AddStep<ci::filesystem::StepCreateStorageDirectories>();
      installer.AddStep<wgt::filesystem::StepCreateSymbolicLink>();
      installer.AddStep<wgt::filesystem::StepWgtCreateIcons>();
      installer.AddStep<ci::pkgmgr::StepGenerateXml>();
      installer.AddStep<ci::pkgmgr::StepRegisterApplication>();
      installer.AddStep<ci::security::StepRegisterSecurity>();
      break;
    }
    case ci::PkgMgrInterface::Type::Update: {
      installer.AddStep<ci::configuration::StepConfigure>();
      installer.AddStep<ci::filesystem::StepUnzip>();
      installer.AddStep<wgt::parse::StepParse>();
      installer.AddStep<ci::security::StepCheckSignature>();
      installer.AddStep<wgt::security::StepCheckSettingsLevel>();
      installer.AddStep<ci::security::StepCheckOldCertificate>();
      installer.AddStep<wgt::encrypt::StepEncryptResources>();
      installer.AddStep<wgt::filesystem::StepWgtResourceDirectory>();
      installer.AddStep<ci::backup::StepOldManifest>();
      installer.AddStep<ci::backup::StepBackupManifest>();
      installer.AddStep<ci::backup::StepBackupIcons>();
      installer.AddStep<ci::backup::StepCopyBackup>();
      installer.AddStep<wgt::filesystem::StepWgtCopyStorageDirectories>();
      installer.AddStep<wgt::filesystem::StepCreateSymbolicLink>();
      installer.AddStep<wgt::filesystem::StepWgtCreateIcons>();
      installer.AddStep<ci::security::StepUpdateSecurity>();
      installer.AddStep<ci::pkgmgr::StepGenerateXml>();
      installer.AddStep<ci::pkgmgr::StepUpdateApplication>();
      break;
    }
    case ci::PkgMgrInterface::Type::Uninstall: {
      installer.AddStep<ci::configuration::StepConfigure>();
      installer.AddStep<ci::parse::StepParse>();
      installer.AddStep<ci::backup::StepBackupManifest>();
      installer.AddStep<ci::pkgmgr::StepUnregisterApplication>();
      installer.AddStep<ci::filesystem::StepRemoveFiles>();
      installer.AddStep<ci::filesystem::StepRemoveIcons>();
      installer.AddStep<ci::security::StepRevokeSecurity>();
      break;
    }
    case ci::PkgMgrInterface::Type::Reinstall: {
      installer.AddStep<ci::configuration::StepConfigure>();
      installer.AddStep<wgt::parse::StepParse>();
      installer.AddStep<ci::backup::StepOldManifest>();
      installer.AddStep<wgt::rds::StepRDSParse>();
      installer.AddStep<wgt::rds::StepRDSModify>();
      installer.AddStep<ci::security::StepUpdateSecurity>();

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
