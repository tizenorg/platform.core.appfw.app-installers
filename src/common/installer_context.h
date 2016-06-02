/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_INSTALLER_CONTEXT_H_
#define COMMON_INSTALLER_CONTEXT_H_

#include <boost/filesystem/path.hpp>

#include <pkgmgr_parser.h>

#include <unistd.h>
#include <sys/types.h>
#include <vcore/Certificate.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "common/external_storage.h"
#include "common/pkgmgr_interface.h"
#include "common/recovery_file.h"
#include "common/request.h"
#include "common/utils/property.h"

#include "manifest_info/account.h"
#include "manifest_info/ime_info.h"

namespace common_installer {

// TODO(t.iwanek): this structure should be unified for manifest handlers of
// wgt and tpk packages
struct ShortcutInfo {
  std::string app_id;
  std::string extra_data;
  std::string extra_key;
  std::string icon;
  std::vector<std::pair<std::string, std::string>> labels;
};

using ShortcutListInfo = std::vector<ShortcutInfo>;


// TODO(p.sikorski): if that structure holds extra information not covered
// in manifest_x, maybe it should hold manifest_x as well?
/**
 * \brief Structure, that holds additional data retrieved from manifest
 * and used during generation of platform manifest (for data that are not
 * available within manifest_x structure
 */
class ExtraManifestData {
 public:
  /** Constructor */
  ExtraManifestData() {}

  Property<AccountInfo> account_info;
  Property<ShortcutListInfo> shortcut_info;
  Property<ImeInfo> ime_info;
};

/**
 * \brief Base class that is used within specific backends to keep additional
 *        information regarding package
 */
class BackendData {
 public:
  /** virtual destructor */
  virtual ~BackendData() { }
};

/**
 * \brief Class represents certificate information
 */
class CertificateInfo {
 public:
  /** author_id (using public key from author certificate) */
  Property<std::string> author_id;
  /** author_certificate */
  Property<ValidationCore::CertificatePtr> author_certificate;
  /** author_intermediate_certificate */
  Property<ValidationCore::CertificatePtr> author_intermediate_certificate;
  /** author_root_certificate */
  Property<ValidationCore::CertificatePtr> author_root_certificate;
  /** distributor_certificate */
  Property<ValidationCore::CertificatePtr> distributor_certificate;
  /** distributor_intermediate_certificate */
  Property<ValidationCore::CertificatePtr> distributor_intermediate_certificate;
  /** distributor_root_certificate */
  Property<ValidationCore::CertificatePtr> distributor_root_certificate;
  /** distributor2_certificate */
  Property<ValidationCore::CertificatePtr> distributor2_certificate;
  /** distributor2_intermediate_certificate */
  Property<ValidationCore::CertificatePtr>
      distributor2_intermediate_certificate;
  /** distributor2_root_certificate */
  Property<ValidationCore::CertificatePtr> distributor2_root_certificate;
};

/**
 * \brief Class used for recovery situation.
 *        It holds pointer to RecoveryFile object.
 */
class RecoveryInfo {
 public:
  /** default constructor */
  RecoveryInfo() { }

  /**
   * Constructor.
   *
   * \param rf RecoveryFile object (pointer to object)
   */
  explicit RecoveryInfo(std::unique_ptr<recovery::RecoveryFile> rf)
      : recovery_file(std::move(rf)) {
  }

  /** pointer to RecoveryFile */
  std::unique_ptr<recovery::RecoveryFile> recovery_file;
};

/**
 * Enumeration for Privilege levels
 */
enum class PrivilegeLevel : int {
  UNTRUSTED  = 0,
  PUBLIC     = 1,
  PARTNER    = 2,
  PLATFORM   = 3
};

/**
 * \brief Helper function. Checks (and compares) passed levels
 *
 * \param required_level level to compare
 * \param allowed_level level to compare
 *
 * \return true, if required_level <= allowed_level
 */
bool SatifiesPrivilegeLevel(PrivilegeLevel required_level,
                   PrivilegeLevel allowed_level);

/**
 * \brief translates privilege level to string
 *
 * \param level privilege level to translate
 *
 * \return translated level (to string)
 */
const char* PrivilegeLevelToString(PrivilegeLevel level);

// TODO(p.sikorski@samsung.com) this class should be divided into:
//  Base Context class
//  CtxInstall class that inherits from Context
//  CtxUninstall class that inherits from Context
//  It is because Uninstallation does not need so many fields.
//  similarly, installation may not need some of them

/**
 * \brief Holds data generated/used by Steps (e.g. pkgid retrieved from
 *        manifest parsing, path to unzipped package).
 *        ContextInstaller is owned by AppInstaller object. Steps holds
 *        “pointers” to ContextInstaller (pointer is initialized in Step
 *        constructor).
 */
class InstallerContext {
 public:
  /** Constructor */
  InstallerContext();

  /** Destructor */
  ~InstallerContext();

  /**
   * \brief package type (string representing name of backend)
   */
  Property<std::string> pkg_type;

  /**
   * \brief In-memory representation of platform xml manifest file
   *        - contains all information needed by tizen application
   *        framework to handle package management (pkgid, icon,
   *        applications, appcontrol, privileges and more)
   */
  Property<manifest_x*> manifest_data;

  /** Pkgmgr-parser plugins data */
  Property<ExtraManifestData> manifest_plugins_data;

  /**
   * \brief In-memory representation of platform xml manifest file
   *        - contains all already stored information needed by tizen
   *        application framework to handle package management (pkgid,
   *        icon, applications, appcontrol, privileges and more)
   *        - this field is set only for update installation
   *        (we need this information for rollback possibility)
   */
  Property<manifest_x*> old_manifest_data;

  /**
   * \brief path to xml platform manifest which was generated according
   *        to maniest_data content */
  Property<boost::filesystem::path> xml_path;

  /**
   * \brief path to backup xml platform manifest which was generated
   *        according to old_maniest_data content (needed for rollback
   *        operations)
   */
  Property<boost::filesystem::path> backup_xml_path;

  /**
   * \brief path to final location of installed package in filesystem
   */
  Property<std::string> pkgid;

  /**
   * \brief package directory path containing app data
   */
  Property<boost::filesystem::path> pkg_path;

  /**
   * \brief file path used for installation or reinstallation process
   */
  Property<boost::filesystem::path> file_path;

  /**
   * \brief tep file path used for TEP installation process
   */
  Property<boost::filesystem::path> tep_path;

  /**
  * \brief boolean property that indicates tep file should be moved or not
  */
  Property<bool> is_tep_move;

  /**
  * \brief boolean property that indicates request is external move or not
  */
	Property<bool> is_move_to_external;

  /**
   * \brief path to temporary directory when package files are unpacked
   *        before coping them to final destination
   */
  Property<boost::filesystem::path> unpacked_dir_path;

  /**
   * \brief uid of user which installation was triggered for
   *        (any normal or globaltizenapp)
   */
  Property<uid_t> uid;

  /**
   * \brief root directory of installation of all packages for user
   *        (${TZ_USER_HOME}/${USER}/apps_rw/) or
   *        tizenglobalapp user (${TZ_SYS_RO_APP} or ${TZ_SYS_RW_APP})
   */
  Property<boost::filesystem::path> root_application_path;

  /**
   * \brief "void*-like" structure to store backend specific
   *        information about installation process
   */
  Property<BackendData*> backend_data;

  /**
   * \brief privilege/visibility level discovered from signature files
   *        - restricts package privileges
   */
  Property<PrivilegeLevel> privilege_level;

  /**
   * \brief certificate information
   */
  Property<CertificateInfo> certificate_info;

  /**
   * \brief information for recovery
   */
  Property<RecoveryInfo> recovery_info;

  /**
   * \brief user type of request (GLOBAL/USER)
   */
  Property<RequestMode> request_mode;

  /**
   * \brief request type received from pkgmgr_installer
   */
  Property<RequestType> request_type;

  /**
   * \brief installation mode (ONLINE / OFFLINE)
   */
  Property<InstallationMode> installation_mode;

  /**
   * \brief preload request received from pkgmgr_installer
   */
  Property<bool> is_preload_request;

  /**
   * \brief force-remove request received from pkgmgr_installer
   */
  Property<bool> force_remove;

  /**
  * \brief Property of vector of files to add
  */
  Property<std::vector<std::string>> files_to_add;

  /**
  * \brief Property of vector of files to modify
  */
  Property<std::vector<std::string>> files_to_modify;

  /**
  * \brief Property of vector of files to delete
  */
  Property<std::vector<std::string>> files_to_delete;

  /**
   * @brief External Storage object if installing in external
   */
  std::unique_ptr<ExternalStorage> external_storage;
};

}  // namespace common_installer

#endif  // COMMON_INSTALLER_CONTEXT_H_
