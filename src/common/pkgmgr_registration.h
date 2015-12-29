// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_PKGMGR_REGISTRATION_H_
#define COMMON_PKGMGR_REGISTRATION_H_

#include <boost/filesystem.hpp>
#include <pkgmgr-info.h>
#include <unistd.h>

#include <string>
#include <vector>

#include "common/typesdefs.h"
#include "common/installer_context.h"
#include "common/wrappers/manifest_x_wrapper.h"

namespace common_installer {

/**
 * \brief Adapter interface for external PkgMgr module used for registering
 *        package into pkgmgr
 *
 * \param xml_path path to generated xml
 * \param pkgid package pkgid
 * \param uid user id
 * \param request_mode current request mode
 *
 * \return true if success
 */
bool RegisterAppInPkgmgr(const ManifestXWrapperPtr& manifest,
                         const boost::filesystem::path& xml_path,
                         const std::string& pkgid,
                         const CertificateInfo& cert_info,
                         uid_t uid,
                         RequestMode request_mode,
                         const boost::filesystem::path& tep_path =
                             boost::filesystem::path());

/**
 * \brief Adapter interface for external PkgMgr module used for upgrading
 *        package within pkgmgr
 *
 * \param xml_path path to generated xml
 * \param pkgid package pkgid
 * \param cert_info certifciate info
 * \param uid user id
 * \param request_mode current request mode
 *
 * \return true if success
 */
bool UpgradeAppInPkgmgr(const ManifestXWrapperPtr& manifest,
                        const boost::filesystem::path& xml_path,
                        const std::string& pkgid,
                        const CertificateInfo& cert_info,
                        uid_t uid,
                        RequestMode request_mode);

/**
 * \brief Adapter interface for external PkgMgr module used for updating
 *        tep info about package within pkgmgr
 *
 * \param tep_path path of tep file
 * \param pkgid package pkgid
 * \param uid user id
 * \param request_mode current request mode
 *
 * \return true if success
 */
bool UpdateTepInfoInPkgmgr(const boost::filesystem::path& tep_path,
                           const std::string& pkgid,
                           uid_t uid,
                           RequestMode request_mode);

/**
 * \brief Adapter interface for external PkgMgr module used for deregistering
 *        package into pkgmgr
 *
 * \param xml_path path to generated xml
 * \param pkgid package pkgid
 * \param uid user id
 * \param request_mode current request mode
 *
 * \return true if success
 */
bool UnregisterAppInPkgmgr(const ManifestXWrapperPtr& manifest,
                           const boost::filesystem::path& xml_path,
                           const std::string& pkgid,
                           uid_t uid,
                           RequestMode request_mode);

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
 * \brief Adapter interface for external PkgMgr module used for checking
 *        if given package is installed/registered
 *
 * \param pkg_id package id
 * \param request_mode request mode
 *
 * \return true if package is installed
 */
bool IsPackageInstalled(const std::string& pkg_id, RequestMode request_mode);

}  // namespace common_installer

#endif  // COMMON_PKGMGR_REGISTRATION_H_
