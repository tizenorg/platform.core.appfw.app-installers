// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_PKGMGR_REGISTRATION_H_
#define COMMON_PKGMGR_REGISTRATION_H_

#include <boost/filesystem.hpp>
#include <pkgmgr-info.h>
#include <unistd.h>

#include <string>

#include "common/installer_context.h"

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
bool RegisterAppInPkgmgr(manifest_x* manifest,
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
bool UpgradeAppInPkgmgr(manifest_x* manifest,
                        const boost::filesystem::path& xml_path,
                        const std::string& pkgid,
                        const CertificateInfo& cert_info,
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
bool UnregisterAppInPkgmgr(manifest_x* manifest,
                           const boost::filesystem::path& xml_path,
                           const std::string& pkgid,
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

}  // namespace common_installer

#endif  // COMMON_PKGMGR_REGISTRATION_H_
