// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/configuration/step_configure.h"

#include <boost/filesystem/path.hpp>

#include <pkgmgr-info.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <tzplatform_config.h>

#include <string>

#include "common/request.h"
#include "common/utils/file_util.h"

namespace bf = boost::filesystem;

namespace common_installer {
namespace configuration {

const char kStrEmpty[] = "";

StepConfigure::StepConfigure(InstallerContext* context, PkgMgrPtr pkgmgr)
    : Step(context),
      pkgmgr_(pkgmgr) {
}

Step::Status StepConfigure::process() {
  SetupRequestMode(pkgmgr_->GetUid());
  SetupRequestType();
  SetupFileCreationMask();

  if (!SetupRootAppDirectory())
    return Status::CONFIG_ERROR;

  switch (pkgmgr_->GetRequestType()) {
    case RequestType::Install:
      context_->file_path.set(pkgmgr_->GetRequestInfo());
      context_->pkgid.set(kStrEmpty);
      if (!pkgmgr_->GetTepPath().empty()) {
        context_->tep_path.set(pkgmgr_->GetTepPath());
        context_->is_tep_move.set(pkgmgr_->GetIsTepMove());
      }
      break;
    case RequestType::Update:
      context_->file_path.set(pkgmgr_->GetRequestInfo());
      context_->pkgid.set(kStrEmpty);
      if (!pkgmgr_->GetTepPath().empty()) {
        context_->tep_path.set(pkgmgr_->GetTepPath());
        context_->is_tep_move.set(pkgmgr_->GetIsTepMove());
      }
      break;
    case RequestType::Uninstall:
      SetupIsForceRemoval();
      context_->pkgid.set(pkgmgr_->GetRequestInfo());
      context_->file_path.set(kStrEmpty);
      break;
    case RequestType::Reinstall:
      context_->unpacked_dir_path.set(pkgmgr_->GetRequestInfo());
      context_->pkgid.set(kStrEmpty);
      context_->file_path.set(kStrEmpty);
      break;
    case RequestType::Clear:
      context_->pkgid.set(pkgmgr_->GetRequestInfo());
      break;
    case RequestType::Delta:
      context_->unpacked_dir_path.set(kStrEmpty);
      context_->pkgid.set(kStrEmpty);
      context_->file_path.set(pkgmgr_->GetRequestInfo());
      break;
    case RequestType::Move:
      context_->pkgid.set(pkgmgr_->GetRequestInfo());
      context_->is_move_to_external.set(pkgmgr_->GetIsMoveToExternal());
      break;
    case RequestType::Recovery:
      context_->file_path.set(pkgmgr_->GetRequestInfo());
      context_->pkgid.set(kStrEmpty);
      break;
    case RequestType::MountInstall:
    case RequestType::MountUpdate:
      context_->file_path.set(pkgmgr_->GetRequestInfo());
      context_->pkgid.set(kStrEmpty);
      if (!pkgmgr_->GetTepPath().empty()) {
        context_->tep_path.set(pkgmgr_->GetTepPath());
        context_->is_tep_move.set(pkgmgr_->GetIsTepMove());
      }
      break;
    case RequestType::ManifestDirectInstall:
    case RequestType::ManifestDirectUpdate: {
      context_->pkgid.set(pkgmgr_->GetRequestInfo());
      bf::path package_directory =
          context_->root_application_path.get() / context_->pkgid.get();
      bf::path xml_path =
          bf::path(getUserManifestPath(context_->uid.get(),
              context_->is_preload_request.get()))
          / bf::path(context_->pkgid.get());
      xml_path += ".xml";
      context_->unpacked_dir_path.set(package_directory);
      context_->pkg_path.set(package_directory);
      context_->xml_path.set(xml_path);
      break;
    }
    case RequestType::DisablePkg:
    case RequestType::EnablePkg:
      context_->pkgid.set(pkgmgr_->GetRequestInfo());
      break;
    default:
      LOG(ERROR) <<
          "Only installation, update and uninstallation is now supported";
      return Status::CONFIG_ERROR;
      break;
  }

  // Record recovery file for update and request modes
  if (pkgmgr_->GetRequestType() == RequestType::Install ||
      pkgmgr_->GetRequestType() == RequestType::Update) {
    std::unique_ptr<recovery::RecoveryFile> recovery_file =
        recovery::RecoveryFile::CreateRecoveryFileForPath(
            GenerateTemporaryPath(
                context_->root_application_path.get() / "recovery"));
    if (!recovery_file) {
      LOG(ERROR) << "Failed to create recovery file";
      return Status::CONFIG_ERROR;
    }
    recovery_file->set_type(pkgmgr_->GetRequestType());
    if (!recovery_file->WriteAndCommitFileContent()) {
      LOG(ERROR) << "Failed to write recovery file";
      return Status::CONFIG_ERROR;
    }
    context_->recovery_info.set(RecoveryInfo(std::move(recovery_file)));
  }

  return Status::OK;
}

Step::Status StepConfigure::precheck() {
  SetupIsPreloadRequest();

  if (getuid() == 0) {
    context_->uid.set(tzplatform_getuid(TZ_SYS_GLOBALAPP_USER));
    if (pkgmgr_->GetRequestType() == RequestType::ManifestDirectInstall ||
        pkgmgr_->GetRequestType() == RequestType::ManifestDirectUpdate) {
      LOG(INFO) << "Allowing installation from root user for "
                   "manifest direct mode.";
    } else if (context_->is_preload_request.get()) {
      LOG(INFO) << "Allowing installation from root user for "
                   "preload request mode.";
    } else {
      LOG(ERROR) << "App-installer should not run with superuser!";
      return Status::OPERATION_NOT_ALLOWED;
    }
  } else {
    context_->uid.set(pkgmgr_->GetUid());
    if (pkgmgr_->GetRequestType() == RequestType::ManifestDirectInstall ||
        pkgmgr_->GetRequestType() == RequestType::ManifestDirectUpdate) {
      if (context_->is_preload_request.get()) {
        LOG(ERROR) << "Direct manifest installation/update that is run from "
                      "non-root user cannot be a preload request";
        return Status::OPERATION_NOT_ALLOWED;
      }
    } else if (context_->is_preload_request.get()) {
      LOG(ERROR) << "Non-root user cannot request preload request mode.";
      return Status::OPERATION_NOT_ALLOWED;
    }
  }
  return Status::OK;
}

Step::Status StepConfigure::clean() {
  // Clean up operations should not be covered by recovery
  // as backup information is being lost during clean up
  context_->recovery_info.get().recovery_file.reset();
  return Status::OK;
}

bool StepConfigure::SetupRootAppDirectory() {
  if (context_->root_application_path.get().empty()) {
    std::string root_app_path =
        GetRootAppPath(context_->is_preload_request.get(), context_->uid.get());
    if (root_app_path.empty())
      return false;

    context_->root_application_path.set(root_app_path);
  }
  if (!boost::filesystem::exists(context_->root_application_path.get())) {
    boost::system:: error_code error;
    boost::filesystem::create_directories(
        context_->root_application_path.get());
    if (error) {
      LOG(ERROR) << "Cannot create directory: "
                 << context_->root_application_path.get();
      return false;
    }
  }
  LOG(INFO) << "AppDir(" << context_->root_application_path.get() << ")";
  return true;
}

void StepConfigure::SetupRequestMode(uid_t uid) {
  context_->request_mode.set(GetRequestMode(uid));
}

void StepConfigure::SetupRequestType() {
  context_->request_type.set(pkgmgr_->GetRequestType());
}

void StepConfigure::SetupFileCreationMask() {
  mode_t old_mask, new_mask;
  old_mask = new_mask = 0;

  switch (context_->request_mode.get()) {
    case RequestMode::USER:
      new_mask = 033;  // results in 744 privileges
      break;
    case RequestMode::GLOBAL:
      new_mask = 022;  // results in 755 privileges
      break;
  }

  old_mask = umask(new_mask);

  LOG(INFO) << "Changed file creation mask from " << std::oct <<  old_mask
      << " to " << std::oct <<  new_mask;
}

void StepConfigure::SetupIsPreloadRequest() {
  context_->is_preload_request.set(pkgmgr_->GetIsPreloadRequest());
}

void StepConfigure::SetupIsForceRemoval() {
  context_->force_remove.set(pkgmgr_->GetIsForceRemoval());
}

}  // namespace configuration
}  // namespace common_installer
