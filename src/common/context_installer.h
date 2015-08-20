/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_CONTEXT_INSTALLER_H_
#define COMMON_CONTEXT_INSTALLER_H_

#include <boost/filesystem/path.hpp>

#include <pkgmgr_parser.h>

#include <unistd.h>
#include <sys/types.h>
#include <vcore/Certificate.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "common/recovery_file.h"
#include "common/request_type.h"
#include "common/utils/property.h"

namespace common_installer {

// TODO(t.iwanek): this structure should be unified for manifest handlers of
// wgt and tpk packages
struct SingleAccountInfo {
  bool multiple_account_support;
  std::vector<std::pair<std::string, std::string>> names;
  std::vector<std::pair<std::string, std::string>> icon_paths;
  std::vector<std::string> capabilities;
  std::string appid;
};

class AccountInfo {
 public:
  AccountInfo() {}
  const std::vector<SingleAccountInfo>& accounts() const {
    return accounts_;
  }
  void set_account(const SingleAccountInfo& single_account) {
    accounts_.push_back(single_account);
  }
 private:
  std::vector<SingleAccountInfo> accounts_;
};

class ExtraManifestData {
 public:
  ExtraManifestData() {}

  Property<AccountInfo> account_info;
};

class BackendData {
 public:
  virtual ~BackendData() { }
};

class CertificateInfo {
 public:
  Property<ValidationCore::CertificatePtr> author_certificate;
};

class RecoveryInfo {
 public:
  RecoveryInfo() { }
  explicit RecoveryInfo(std::unique_ptr<recovery::RecoveryFile> rf)
      : recovery_file(std::move(rf)) {
  }

  std::unique_ptr<recovery::RecoveryFile> recovery_file;
};

enum class PrivilegeLevel : int {
  UNTRUSTED  = 0,
  PUBLIC     = 1,
  PARTNER    = 2,
  PLATFORM   = 3
};

bool SatifiesPrivilegeLevel(PrivilegeLevel required_level,
                   PrivilegeLevel allowed_level);
const char* PrivilegeLevelToString(PrivilegeLevel level);

// TODO(p.sikorski@samsung.com) this class should be divided into:
//  Base Context class
//  CtxInstall class that inherits from Context
//  CtxUninstall class that inherits from Context
//  It is because Uninstallation does not need so many fields.
//  similarly, installation may not need some of them
class ContextInstaller {
 public:
  ContextInstaller();
  ~ContextInstaller();

  // package_type
  Property<std::string> pkg_type;

  //  manifest information used to generate xml file
  Property<manifest_x*> manifest_data;

  // Pkgmgr-parser plugins data
  Property<ExtraManifestData> manifest_plugins_data;

  //  manifest information used to revert an update
  Property<manifest_x*> old_manifest_data;

  // path to manifest xml file used to register data in databases
  Property<boost::filesystem::path> xml_path;

  // path to old manifest xml while updating
  Property<boost::filesystem::path> backup_xml_path;

  // pkgid used for update or uninstallation processing
  Property<std::string> pkgid;

  // package directory path containing app data
  Property<boost::filesystem::path> pkg_path;

  // file path used for installation or reinstallation process
  Property<boost::filesystem::path> file_path;

  // directory path where app data are temporarily extracted
  Property<boost::filesystem::path> unpacked_dir_path;

  // uid of the user that request the operation
  Property<uid_t> uid;

  // path for the applications root directory
  Property<boost::filesystem::path> root_application_path;

  // Backend specific data
  Property<BackendData*> backend_data;

  // request privilege level of package
  Property<PrivilegeLevel> privilege_level;

  // certificate information
  Property<CertificateInfo> certificate_info;

  // information for recovery
  Property<RecoveryInfo> recovery_info;
};

}  // namespace common_installer

#endif  // COMMON_CONTEXT_INSTALLER_H_
