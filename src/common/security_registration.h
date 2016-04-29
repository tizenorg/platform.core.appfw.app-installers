// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_SECURITY_REGISTRATION_H_
#define COMMON_SECURITY_REGISTRATION_H_

#include <boost/filesystem/path.hpp>

#include <sys/types.h>

#include <string>
#include <vector>

#include "common/installer_context.h"

namespace common_installer {

enum class SecurityAppInstallType { None, Local, Global, Preload };

/**
 * Adapter interface for external Security module.
 *
 * Adapter interface for external Security module used for registering
 * application to security context
 *
 * \param app_id id of given application
 * \param pkg_id id of given package
 * \param author_id unique author id of given package
 * \param api_version api-version of given package
 * \param path path of installed package
 * \param uid uid
 * \param privileges pointer to manifest structure
 * \param error_message extra/detailed error message
 *
 * \return true if success
 */
bool RegisterSecurityContext(const std::string& app_id,
    const std::string& pkg_id, const std::string& author_id,
    const std::string& api_version, SecurityAppInstallType type,
    const boost::filesystem::path& path, uid_t uid,
    const std::vector<std::string>& privileges,
    std::string* error_message);

/**
 * Adapter interface for external Security module.
 *
 * Adapter interface for external Security module used for unregistering
 * application from security context
 *
 * \param app_id id of given application
 * \param pkg_id id of given package
 * \param uid uid
 * \param error_message extra/detailed error message
 *
 * \return true if success
 */
bool UnregisterSecurityContext(const std::string& app_id,
    const std::string& pkg_id, uid_t uid, std::string* error_message);

/**
 * Adapter interface for external Security module.
 *
 * Adapter interface for external Security module used for registering
 * package to security context
 *
 * \param pkg_id pkgid of given package
 * \param path path of installed package
 * \param uid uid
 * \param cert_info pointer to certificate info
 * \param manifest pointer to manifest structure
 * \param error_message extra/detailed error message
 *
 * \return true if success
 */
bool RegisterSecurityContextForManifest(const std::string& pkg_id,
    const boost::filesystem::path& path, uid_t uid,
    common_installer::CertificateInfo* cert_info, manifest_x* manifest,
    std::string* error_message);

/**
 * Adapter interface for external Security module.
 *
 * Adapter interface for external Security module used for unregistering
 * package from security context
 *
 * \param pkg_id pkgid of given package
 * \param uid uid
 * \param manifest pointer to manifest structure
 * \param error_message extra/detailed error message
 *
 * \return true if success
 */
bool UnregisterSecurityContextForManifest(const std::string& pkg_id, uid_t uid,
    manifest_x* manifest, std::string* error_message);

/**
 * Adapter interface for external Security module.
 *
 * Adapter interface for external Security module used for unregistering
 * package from security context
 *
 * \param pkg_id pkgid of given package
 * \param uid uid
 * \param error_message extra/detailed error message
 *
 * \return true if success
 */
bool UnregisterSecurityContextForPkgId(const std::string& pkg_id, uid_t uid,
    std::string* error_message);

/**
 * Adapter interface for external Security module.
 *
 * Adapter interface for external Security module used for registering
 * package path to security context
 *
 * \param pkg_id pkgid of given package
 * \param path path for registering
 * \param uid uid
 * \param error_message extra/detailed error message
 *
 * \return true if success
 */
bool RegisterSecurityContextForPath(const std::string &pkg_id,
    const boost::filesystem::path& path, uid_t uid,
    std::string* error_message);

}  // namespace common_installer

#endif  // COMMON_SECURITY_REGISTRATION_H_
