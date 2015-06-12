// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_rds_modify.h"

#include <boost/system/error_code.hpp>

#include "common/utils/file_util.h"
#include "common/utils/logging.h"

namespace wgt {
namespace rds_modify {

namespace bf = boost::filesystem;
namespace bs = boost::system;
namespace cu = common_installer;

StepRDSModify::StepRDSModify(common_installer::ContextInstaller* context)
    : Step(context),
      backend_data_(nullptr) {}

common_installer::Step::Status StepRDSModify::precheck() {
  if (context_->unpacked_dir_path.get().empty()) {
    LOG(ERROR) << "unpacked dir path is not set";
    return common_installer::Step::Status::ERROR;
  }
  if (!bf::exists(context_->unpacked_dir_path.get())) {
    LOG(ERROR) << "unpacked_dir_path ("
               << context_->unpacked_dir_path.get()
               << ") path does not exist";
    return common_installer::Step::Status::ERROR;
  }
  if (context_->pkgid.get().empty()) {
    LOG(ERROR) << "pkgid is not set";
    return common_installer::Step::Status::ERROR;
  }
  if (!context_->manifest_data.get()) {
    LOG(ERROR) << "no manifest info available";
    return common_installer::Step::Status::ERROR;
  }
  // TODO(w.kosowicz): check if config of installed app was encrypted
  backend_data_ = static_cast<WgtBackendData*>(context_->backend_data.get());
  if (!backend_data_) {
    LOG(ERROR) << "no backend data";
    return common_installer::Step::Status::ERROR;
  }
  return common_installer::Step::Status::OK;
}

common_installer::Step::Status StepRDSModify::process() {
  LOG(INFO) << "entered process of step modify";
  if (!SetUpTempBackupDir()) {
    LOG(ERROR) << "unable to setup temp directory";
    return common_installer::Step::Status::ERROR;
  }
  context_->pkg_path.set(
        context_->root_application_path.get() /context_->pkgid.get());
  bf::path install_path = context_->pkg_path.get() / "res" / "wgt";
  bf::path unzip_path = context_->unpacked_dir_path.get();
  if (!AddFiles(unzip_path, install_path) ||
     !ModifyFiles(unzip_path, install_path) ||
     !DeleteFiles(install_path)) {
    LOG(ERROR) << "error during file operation";
    return common_installer::Step::Status::ERROR;
  }
  return common_installer::Step::Status::OK;
}

common_installer::Step::Status StepRDSModify::undo() {
  RestoreFiles();
  return common_installer::Step::Status::OK;
}

common_installer::Step::Status StepRDSModify::clean() {
  if (bf::exists(backup_temp_dir_))
    bf::remove_all(backup_temp_dir_);
  return common_installer::Step::Status::OK;
}

bool StepRDSModify::AddFiles(bf::path unzip_path, bf::path install_path) {
  LOG(INFO) << "about to add files";
  bs::error_code error;
  for (const auto& file : backend_data_->files_to_add.get()) {
    if (!PerformBackup(file, Operation::ADD)) {
      LOG(ERROR) << "unable to perform backup of added file";
      return false;
    }
    bf::path temp_install_path(install_path / file);
    if (bf::is_directory(temp_install_path)) {
      if (!bf::exists(temp_install_path) &&
         (!cu::CreateDir(temp_install_path))) {
        LOG(ERROR) << "unable to create dir for temp backup data";
        return false;
      }
    } else {
      if (!bf::exists(temp_install_path.parent_path()) &&
          !cu::CreateDir(temp_install_path.parent_path())) {
        LOG(ERROR) << "unable to create dir for temp backup data";
        return false;
      }
      bf::path temp_unzip_path(unzip_path / file);
      bf::copy_file(temp_unzip_path, temp_install_path, error);
      if (error) {
        LOG(ERROR) << "unable to add file " << error.message();
        return false;
      }
    }
  }
  return true;
}

bool StepRDSModify::ModifyFiles(bf::path unzip_path, bf::path install_path) {
  LOG(INFO) << "about to modify files";
  bs::error_code error;
  for (const auto& file : backend_data_->files_to_modify.get()) {
    bf::path temp_install_path(install_path / file);
    bf::path temp_unzip_path(unzip_path / file);
    if (!PerformBackup(file, Operation::MODIFY)) {
      LOG(ERROR) << "unable to perform backup of to be modified file";
      return false;
    }
    bf::copy_file(temp_unzip_path, temp_install_path,
                  bf::copy_option::overwrite_if_exists, error);
    if (error) {
      LOG(ERROR) << "unable to modify file " << error.message();
      return false;
    }
  }
  return true;
}

bool StepRDSModify::DeleteFiles(bf::path install_path) {
  LOG(INFO) << "about to delete files";
  bs::error_code error;
  for (const auto& file : backend_data_->files_to_delete.get()) {
    if (!PerformBackup(file, Operation::DELETE)) {
      LOG(ERROR) << "unable to perform backup of to be deleted file";
      return false;
    }
    bf::remove(install_path / file, error);
    if (error) {
      LOG(ERROR) <<"unable to delete files " << error.message();
      return false;
    }
  }
  return true;
}

bool StepRDSModify::SetUpTempBackupDir() {
  LOG(INFO) << "about to setup tmp backup dir";
  bs::error_code error;
  backup_temp_dir_ = "/tmp/" /
      bf::unique_path("%%%%-%%%%-%%%%-%%%%", error);
  if (error || !cu::CreateDir(backup_temp_dir_)) {
    LOG(ERROR) << "unable to create backup data temp dir";
    return false;
  }
  return true;
}

bool StepRDSModify::PerformBackup(std::string relative_path,
                                  Operation operation) {
  if (backup_temp_dir_.empty())
    return false;
  if (operation == Operation::DELETE || operation == Operation::MODIFY) {
    bf::path app_path = context_->pkg_path.get() /
        context_->manifest_data.get()->mainapp_id;;
    bf::path source_path = app_path  / relative_path;
    if (bf::is_directory(source_path)) {
      if (!cu::CreateDir(backup_temp_dir_ / relative_path)) {
        LOG(ERROR) << "unable to create dir for temp backup data";
        return false;
      }
    } else {
      bs::error_code error;
      bf::path tmp_dest_path = backup_temp_dir_ / relative_path;
      if (!bf::exists((tmp_dest_path).parent_path()) &&
        (!cu::CreateDir((tmp_dest_path).parent_path()))) {
        LOG(ERROR) << "unable to create dir for temp backup data";
        return false;
      }
      bf::copy_file(source_path, tmp_dest_path, error);
      if (error) {
        LOG(ERROR) << "unable to backup file " << error.message();
        return false;
      }
    }
  }
  success_modifications_.push_back(std::make_pair(relative_path, operation));
  return true;
}

void StepRDSModify::RestoreFiles() {
  LOG(ERROR) << "error occured about to restore files";
  bf::path app_path(context_->pkg_path.get());
  for (std::pair<std::string, Operation>& modification :
       success_modifications_) {
    bf::path source_path(backup_temp_dir_ / modification.first);
    bf::path destination_path(app_path / modification.first);
    if (modification.second == Operation::ADD) {
      if (bf::is_directory(source_path)) {
        bf::remove_all(destination_path);
      } else {
        bf::remove(destination_path);
      }
    } else if (modification.second == Operation::MODIFY) {
      bf::copy_file(source_path, destination_path,
                    bf::copy_option::overwrite_if_exists);
    } else {
      if (bf::is_directory(source_path)) {
        cu::CreateDir(destination_path);
      } else {
        bf::copy_file(source_path, destination_path,
                      bf::copy_option::overwrite_if_exists);
      }
    }
  }
  // after files are restore delete temporary location
  bf::remove_all(backup_temp_dir_);
}

}  // namespace rds_modify
}  // namespace wgt
