// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_PKGMGR_QUERY_H_
#define COMMON_PKGMGR_QUERY_H_

#include <unistd.h>

#include <string>
#include <vector>

#include "common/request.h"

namespace common_installer {

/**
 * \brief Adapter interface for external PkgMgr module used for getting
 *        certificate information for given package
 *
 * \param pkgid package pkgid
 * \param uid user id
 *
 * \return returns certificate information
 */
std::string QueryCertificateAuthorCertificate(const std::string& pkgid,
                                              uid_t uid);

/**
 * \brief Adapter interface for external PkgMgr module used for getting
 *        tizen extension package path for given package
 *
 * @param pkgid package id
 * @param uid user id
 * @return path or empty
 */
std::string QueryTepPath(const std::string& pkgid, uid_t uid);

/**
 * \brief Adapter interface for external PkgMgr module used for getting
 *        zip mount path for given package
 *
 * @param pkgid package id
 * @param uid user id
 * @return path or empty
 */
std::string QueryZipMountFile(const std::string& pkgid, uid_t uid);

/**
 * \brief Adapter interface for external PkgMgr module used for getting
 *        list of appids for given package
 *
 * \param pkg_id[in] package pkgid
 * \param result[out] Output
 * \param uid user id
 *
 * \return true if success
 */
bool QueryAppidsForPkgId(const std::string& pkg_id,
                         std::vector<std::string>* result, uid_t uid);

/**
 * \brief Adapter interface for external PkgMgr module used for getting
 *        list of privileges for given package
 *
 * \param pkg_id id of package
 * \param uid user id
 * \param result result - privileges
 *
 * \return true if success
 */
bool QueryPrivilegesForPkgId(const std::string& pkg_id, uid_t uid,
                             std::vector<std::string>* result);

/**
 * \brief Adapter interface for external PkgMgr module used for getting
 *        storage for given package
 *
 * \param pkg_id id of package
 * \param uid user id
 *
 * \return storage name of empty if not installed
 */
std::string QueryStorageForPkgId(const std::string& pkg_id, uid_t uid);

/**
 * \brief Adapter interface for external PkgMgr module used for checking
 *        if given package is installed/registered
 *
 * \param pkg_id package id
 * \param request_mode request mode
 *
 * \return true if package is installed
 */
bool QueryIsPackageInstalled(const std::string& pkg_id,
                             RequestMode request_mode);

/**
 * \brief Adapter interface for external PkgMgr module used for checking
 *        if given package is installed/registered per given uid only
 *
 * \param pkg_id package id
 * \param uid user id
 *
 * \return true if package is installed
 */
bool QueryIsPackageInstalled(const std::string& pkg_id, uid_t uid);

}  // namespace common_installer

#endif  // COMMON_PKGMGR_QUERY_H_
