// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_RDS_STEP_RDS_MODIFY_H_
#define COMMON_RDS_STEP_RDS_MODIFY_H_

#include <boost/filesystem.hpp>
#include <common/step/step.h>
#include <string>
#include <utility>
#include <vector>

#include "common/installer_context.h"

namespace common_installer {

/**
 * \brief Step that apply RDS modification during reinstallation process
 */
class StepRDSModify : public Step {
 public:
  /**
   * \brief Explicit constructor
   *
   * \param context Installer context
   */
  explicit StepRDSModify(common_installer::InstallerContext* context);

  /**
   * \brief
   *
   * \return
   */
  Status process() override;

  /**
   * \brief Remove files from temporary location
   *
   * \return Status::OK
   */
  Status clean() override;

  /**
   * \brief Restore files to the state from before RDS installation
   *
   * \return Status::OK
   */
  Status undo() override;

  /**
   * \brief
   *
   * \return Status::ERROR when manifest is missing, pkgid is missing,
   *         or when path to the unpacked directory is missing or not exist,
   *         Status::OK otherwise
   */
  Status precheck() override;

 protected:
  /**
   * \brief Parse .rds_delta file
   *
   * \return std::string
   */
  virtual std::string GetAppPath() = 0;

 private:
  enum class Operation {
    ADD,
    MODIFY,
    DELETE
  };

  bool AddFiles(boost::filesystem::path unzip_path,
                boost::filesystem::path install_path);
  bool ModifyFiles(boost::filesystem::path unzip_path,
                   boost::filesystem::path install_path);
  bool DeleteFiles(boost::filesystem::path install_path);
  bool SetUpTempBackupDir();
  void DeleteTempBackupDir();
  bool PerformBackup(std::string relative_path, Operation operation);
  void RestoreFiles();

  BackendData* backend_data_;
  std::vector<std::pair<std::string, Operation>> success_modifications_;
  boost::filesystem::path backup_temp_dir_;
  std::vector<std::string> files_to_modify_;
  std::vector<std::string> files_to_add_;
  std::vector<std::string> files_to_delete_;
};

}  // namespace common_installer

#endif  // COMMON_RDS_STEP_RDS_MODIFY_H_
