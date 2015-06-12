// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/step_rds_modify.h"

#include <boost/system/error_code.hpp>

#include "utils/file_util.h"

namespace wgt {
namespace rds_modify {

namespace bf = boost::filesystem;
namespace bs = boost::system;
namespace cu = common_installer::utils;

StepRDSModify::StepRDSModify(common_installer::ContextInstaller* context)
    : Step(context),
      backend_data_(nullptr),
      backup_temp_dir_(bf::unique_path()) {}

common_installer::Step::Status StepRDSModify::precheck() {
  backend_data_ = static_cast<WgtBackendData*>(context_->backend_data.get());
  if (!backend_data_)
    return common_installer::Step::Status::ERROR;
  return common_installer::Step::Status::OK;
}

common_installer::Step::Status StepRDSModify::process() {
  if (!SetUpTempBackupDir())
    return common_installer::Step::Status::ERROR;
  context_->pkg_path.set(
        context_->root_application_path.get() /context_->pkgid.get());
  bf::path install_path = context_->pkg_path.get();
  bf::path unzip_path = context_->unpacked_dir_path.get();
  if (!AddFiles(unzip_path, install_path) ||
     !ModifyFiles(unzip_path, install_path) ||
     !DeleteFiles(install_path))
    return common_installer::Step::Status::ERROR;
  return common_installer::Step::Status::OK;
}

common_installer::Step::Status StepRDSModify::undo() {
  RestoreFiles();
  return common_installer::Step::Status::OK;
}

bool StepRDSModify::AddFiles(bf::path unzip_path, bf::path install_path) {
  bs::error_code error;
  for (const auto& file : backend_data_->files_to_add.get()) {
    if (!PerformBackup(file, Operation::ADD))
      return false;
    bf::path temp_install_path(install_path / file);
    if (bf::is_directory(temp_install_path)) {
      if (!bf::exists(temp_install_path) && (!cu::CreateDir(temp_install_path)))
        return false;
    } else {
      if (!bf::exists(temp_install_path.parent_path()) &&
          !cu::CreateDir(temp_install_path.parent_path()))
        return false;
      bf::path temp_unzip_path(unzip_path / file);
      bf::copy_file(temp_unzip_path, temp_install_path, error);
    }
    if (error)
      return false;
  }
  return true;
}

bool StepRDSModify::ModifyFiles(bf::path unzip_path, bf::path install_path) {
  bs::error_code error;
  for (const auto& file : backend_data_->files_to_modify.get()) {
    bf::path temp_install_path(install_path / file);
    bf::path temp_unzip_path(unzip_path / file);
    if (!PerformBackup(file, Operation::MODIFY))
      return false;
    bf::copy_file(temp_unzip_path, temp_install_path,
                  bf::copy_option::overwrite_if_exists, error);
    if (error)
      return false;
  }
  return true;
}

bool StepRDSModify::DeleteFiles(bf::path install_path) {
  bs::error_code error;
  for (const auto& file : backend_data_->files_to_delete.get()) {
    if (!PerformBackup(file, Operation::DELETE))
      return false;
    bf::remove(install_path / file, error);
    if (error)
      return false;
  }
  return true;
}

bool StepRDSModify::SetUpTempBackupDir() {
  bs::error_code error;
  backup_temp_dir_ = "/tmp/" /
      bf::unique_path("%%%%-%%%%-%%%%-%%%%", error) / "/";
  if (error || !cu::CreateDir(backup_temp_dir_))
    return false;
  return true;
}

bool StepRDSModify::PerformBackup(std::string relative_path,
                                  Operation operation) {
  if (backup_temp_dir_.empty())
    return false;
  if (operation == Operation::DELETE || operation == Operation::MODIFY) {
    bf::path app_path = context_->pkg_path.get();
    bf::path source_path = app_path / "/" / relative_path;
    if (bf::is_directory(source_path)) {
      if (!cu::CreateDir(backup_temp_dir_ / relative_path))
        return false;
    } else {
      bs::error_code error;
      bf::path tmp_dest_path = backup_temp_dir_ / relative_path;
      if (!bf::exists((tmp_dest_path).parent_path()) &&
        (!cu::CreateDir((tmp_dest_path).parent_path())))
        return false;
      bf::copy_file(source_path, tmp_dest_path, error);
      if (error)
        return false;
    }
  }
  success_modifications_.push_back(std::make_pair(relative_path, operation));
  return true;
}

void StepRDSModify::RestoreFiles() {
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
