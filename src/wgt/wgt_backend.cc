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
#include "common/step/step_create_storage_directories.h"
#include "common/step/step_generate_xml.h"
#include "common/step/step_parse.h"
#include "common/step/step_register_app.h"
#include "common/step/step_remove_files.h"
#include "common/step/step_revoke_security.h"
#include "common/step/step_register_security.h"
#include "common/step/step_old_manifest.h"
#include "common/step/step_check_signature.h"
#include "common/step/step_unregister_app.h"
#include "common/step/step_unzip.h"
#include "common/step/step_update_app.h"
#include "common/step/step_update_security.h"

#include "wgt/step/step_create_symbolic_link.h"
#include "wgt/step/step_parse.h"
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
      installer.AddStep<ci::configure::StepConfigure>();
      installer.AddStep<ci::unzip::StepUnzip>();
      installer.AddStep<wgt::parse::StepParse>();
      installer.AddStep<ci::signature::StepCheckSignature>();
      installer.AddStep<ci::copy::StepCopy>();
      installer.AddStep<ci::create_storage::StepCreateStorageDirectories>();
      installer.AddStep<wgt::symbolic_link::StepCreateSymbolicLink>();
      installer.AddStep<ci::generate_xml::StepGenerateXml>();
      installer.AddStep<ci::register_app::StepRegisterApplication>();
      installer.AddStep<ci::security::StepRegisterSecurity>();
      break;
    }
    case ci::PkgMgrInterface::Type::Update: {
      installer.AddStep<ci::configure::StepConfigure>();
      installer.AddStep<ci::unzip::StepUnzip>();
      installer.AddStep<wgt::parse::StepParse>();
      installer.AddStep<ci::signature::StepCheckSignature>();
      installer.AddStep<ci::old_manifest::StepOldManifest>();
      installer.AddStep<ci::backup_manifest::StepBackupManifest>();
      installer.AddStep<ci::backup_icons::StepBackupIcons>();
      installer.AddStep<ci::copy_backup::StepCopyBackup>();
      installer.AddStep<ci::copy_storage::StepCopyStorageDirectories>();
      installer.AddStep<wgt::symbolic_link::StepCreateSymbolicLink>();
      installer.AddStep<ci::update_security::StepUpdateSecurity>();
      installer.AddStep<ci::generate_xml::StepGenerateXml>();
      installer.AddStep<ci::update_app::StepUpdateApplication>();
      break;
    }
    case ci::PkgMgrInterface::Type::Uninstall: {
      installer.AddStep<ci::configure::StepConfigure>();
      installer.AddStep<ci::parse::StepParse>();
      installer.AddStep<ci::unregister_app::StepUnregisterApplication>();
      installer.AddStep<ci::remove::StepRemoveFiles>();
      installer.AddStep<ci::revoke_security::StepRevokeSecurity>();
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
