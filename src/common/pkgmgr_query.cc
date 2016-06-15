// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/pkgmgr_query.h"

#include <manifest_parser/utils/logging.h>
#include <pkgmgr_installer.h>
#include <pkgmgr-info.h>
#include <pkgmgr_parser.h>

namespace {

int PkgmgrForeachAppCallback(const pkgmgrinfo_appinfo_h handle,
                              void *user_data) {
  auto* data = static_cast<std::vector<std::string>*>(user_data);
  char* app_id = nullptr;
  if (pkgmgrinfo_appinfo_get_appid(handle, &app_id) != PMINFO_R_OK)
    return PMINFO_R_ERROR;
  data->emplace_back(app_id);
  return PMINFO_R_OK;
}

int PkgmgrForeachPrivilegeCallback(const char* privilege_name,
                                   void* user_data) {
  auto* data = static_cast<std::vector<std::string>*>(user_data);
  data->emplace_back(privilege_name);
  return PMINFO_R_OK;
}

}  // namespace

namespace common_installer {


std::string QueryCertificateAuthorCertificate(const std::string& pkgid,
                                              uid_t uid) {
  pkgmgrinfo_certinfo_h handle;
  int ret = pkgmgrinfo_pkginfo_create_certinfo(&handle);
  if (ret != PMINFO_R_OK) {
    LOG(DEBUG) << "pkgmgrinfo_pkginfo_create_certinfo failed with error: "
               << ret;
    return {};
  }
  ret = pkgmgrinfo_pkginfo_load_certinfo(pkgid.c_str(), handle, uid);
  if (ret != PMINFO_R_OK) {
    LOG(DEBUG) << "pkgmgrinfo_pkginfo_load_certinfo failed with error: " << ret;
    pkgmgrinfo_pkginfo_destroy_certinfo(handle);
    return {};
  }
  const char* author_cert = nullptr;
  ret = pkgmgrinfo_pkginfo_get_cert_value(handle, PMINFO_AUTHOR_SIGNER_CERT,
                                          &author_cert);
  if (ret != PMINFO_R_OK) {
    LOG(DEBUG) << "pkgmgrinfo_pkginfo_get_cert_value failed with error: "
               << ret;
    pkgmgrinfo_pkginfo_destroy_certinfo(handle);
    return {};
  }
  std::string old_author_certificate;
  if (author_cert)
    old_author_certificate = author_cert;
  pkgmgrinfo_pkginfo_destroy_certinfo(handle);
  return old_author_certificate;
}

std::string QueryTepPath(const std::string& pkgid, uid_t uid) {
  pkgmgrinfo_pkginfo_h package_info;
  if (pkgmgrinfo_pkginfo_get_usr_pkginfo(pkgid.c_str(), uid, &package_info)
      != PMINFO_R_OK)
    return {};
  char* tep_name = nullptr;
  int ret = pkgmgrinfo_pkginfo_get_tep_name(package_info, &tep_name);
  if (ret != PMINFO_R_OK) {
    LOG(DEBUG) << "pkgmgrinfo_pkginfo_get_tep_name failed with error: "
               << ret;
    pkgmgrinfo_pkginfo_destroy_pkginfo(package_info);
    return {};
  }
  std::string tep_name_value;
  if (tep_name)
    tep_name_value = tep_name;
  pkgmgrinfo_pkginfo_destroy_pkginfo(package_info);
  return tep_name_value;
}

std::string QueryZipMountFile(const std::string& pkgid, uid_t uid) {
  pkgmgrinfo_pkginfo_h package_info;
  if (pkgmgrinfo_pkginfo_get_usr_pkginfo(pkgid.c_str(), uid, &package_info)
      != PMINFO_R_OK)
    return {};
  char* zip_mount_file = nullptr;
  int ret = pkgmgrinfo_pkginfo_get_zip_mount_file(package_info,
          &zip_mount_file);
  if (ret != PMINFO_R_OK) {
    LOG(DEBUG) << "pkgmgrinfo_pkginfo_get_zip_mount_file failed with error: "
               << ret;
    pkgmgrinfo_pkginfo_destroy_pkginfo(package_info);
    return {};
  }
  std::string zip_mount_file_value;
  if (zip_mount_file)
    zip_mount_file_value = zip_mount_file;
  pkgmgrinfo_pkginfo_destroy_pkginfo(package_info);
  return zip_mount_file_value;
}

bool QueryAppidsForPkgId(const std::string& pkg_id,
                         std::vector<std::string>* result, uid_t uid) {
  pkgmgrinfo_pkginfo_h package_info;
  if (pkgmgrinfo_pkginfo_get_usr_pkginfo(pkg_id.c_str(), uid, &package_info)
      != PMINFO_R_OK) {
    return false;
  }

  bool ret = pkgmgrinfo_appinfo_get_usr_list(package_info, PMINFO_ALL_APP,
      &PkgmgrForeachAppCallback, result, uid) == PMINFO_R_OK;
  pkgmgrinfo_pkginfo_destroy_pkginfo(package_info);
  return ret;
}

bool QueryPrivilegesForPkgId(const std::string& pkg_id, uid_t uid,
                             std::vector<std::string>* result) {
  pkgmgrinfo_pkginfo_h package_info;
  if (pkgmgrinfo_pkginfo_get_usr_pkginfo(pkg_id.c_str(), uid, &package_info)
      != PMINFO_R_OK) {
    return false;
  }

  bool ret = pkgmgrinfo_pkginfo_foreach_privilege(package_info,
      &PkgmgrForeachPrivilegeCallback, result) == PMINFO_R_OK;
  pkgmgrinfo_pkginfo_destroy_pkginfo(package_info);
  return ret;
}

std::string QueryStorageForPkgId(const std::string& pkg_id, uid_t uid) {
  // initial & default : internal
  std::string installed_location = "installed_internal";
  pkgmgrinfo_pkginfo_h package_info;
  if (pkgmgrinfo_pkginfo_get_usr_pkginfo(pkg_id.c_str(), uid, &package_info)
      != PMINFO_R_OK) {
    return installed_location;
  }

  pkgmgrinfo_installed_storage storage;
  bool ok = pkgmgrinfo_pkginfo_get_installed_storage(package_info,
      &storage) == PMINFO_R_OK;
  pkgmgrinfo_pkginfo_destroy_pkginfo(package_info);

  if (!ok)
    return installed_location;

  if (storage == PMINFO_EXTERNAL_STORAGE)
    installed_location = "installed_external";

  return installed_location;
}

bool QueryIsPackageInstalled(const std::string& pkg_id,
                             RequestMode request_mode) {
  pkgmgrinfo_pkginfo_h handle;
  int ret = pkgmgrinfo_pkginfo_get_usr_pkginfo(pkg_id.c_str(), getuid(),
                                               &handle);
  if (ret != PMINFO_R_OK) {
    if (ret != PMINFO_R_ENOENT)
      LOG(ERROR) << "Failed to call pkgmgrinfo_pkginfo_get_usr_pkginfo";
    return false;
  }
  bool is_global = false;
  if (pkgmgrinfo_pkginfo_is_for_all_users(handle, &is_global) != PMINFO_R_OK) {
    LOG(ERROR) << "pkgmgrinfo_pkginfo_is_for_all_users failed";
    pkgmgrinfo_pkginfo_destroy_pkginfo(handle);
    return false;
  }
  if (request_mode != RequestMode::GLOBAL && is_global) {
    pkgmgrinfo_pkginfo_destroy_pkginfo(handle);
    return false;
  }

  pkgmgrinfo_pkginfo_destroy_pkginfo(handle);
  return true;
}

bool QueryIsPackageInstalled(const std::string& pkg_id, uid_t uid) {
  pkgmgrinfo_pkginfo_h handle;
  int ret = pkgmgrinfo_pkginfo_get_usr_pkginfo(pkg_id.c_str(), uid, &handle);
  if (ret != PMINFO_R_OK) {
    if (ret != PMINFO_R_ENOENT)
      LOG(ERROR) << "Failed to call pkgmgrinfo_pkginfo_get_usr_pkginfo";
    return false;
  }

  bool is_global = false;
  if (pkgmgrinfo_pkginfo_is_for_all_users(handle, &is_global) != PMINFO_R_OK) {
    LOG(ERROR) << "pkgmgrinfo_pkginfo_is_for_all_users failed";
    pkgmgrinfo_pkginfo_destroy_pkginfo(handle);
    return false;
  }

  if (uid == GLOBAL_USER && is_global) {
    pkgmgrinfo_pkginfo_destroy_pkginfo(handle);
    return true;
  }

  if (uid != GLOBAL_USER && is_global) {
    pkgmgrinfo_pkginfo_destroy_pkginfo(handle);
    return false;
  }

  pkgmgrinfo_pkginfo_destroy_pkginfo(handle);
  return true;
}

}  // namespace common_installer
