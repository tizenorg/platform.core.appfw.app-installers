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
#include "common/typesdefs.h"

namespace common_installer {

/**
 * Adapter interface for external Security module.
 *
 * Adapter interface for external Security module used for registering
 * application to security context
 *
 * \param app_id id of given application
 * \param pkg_id id of given package
 * \param path path of installed package
 * \param uid uid
 * \param privileges pointer to manifest structure
 *
 * \return true if success
 */
bool RegisterSecurityContext(const std::string& app_id,
    const std::string& pkg_id, const boost::filesystem::path& path, uid_t uid,
    const std::vector<std::string>& privileges);

/**
 * Adapter interface for external Security module.
 *
 * Adapter interface for external Security module used for unregistering
 * application from security context
 *
 * \param app_id id of given application
 * \param pkg_id id of given package
 * \param uid uid
 *
 * \return true if success
 */
bool UnregisterSecurityContext(const std::string& app_id,
    const std::string& pkg_id, uid_t uid);

/**
 * Adapter interface for external Security module.
 *
 * Adapter interface for external Security module used for registering
 * package to security context
 *
 * \param pkg_id pkgid of given package
 * \param path path of installed package
 * \param uid uid
 * \param manifest pointer to manifest structure
 *
 * \return true if success
 */
bool RegisterSecurityContextForManifest(const std::string& pkg_id,
                                        const boost::filesystem::path& path,
                                        uid_t uid,
                                        const ManifestXWrapperPtr& manifest);

/**
 * Adapter interface for external Security module.
 *
 * Adapter interface for external Security module used for unregistering
 * package from security context
 *
 * \param pkg_id pkgid of given package
 * \param uid uid
 * \param manifest pointer to manifest structure
 *
 * \return true if success
 */
bool UnregisterSecurityContextForManifest(const std::string& pkg_id, uid_t uid,
    const ManifestXWrapperPtr& manifest);

}  // namespace common_installer

#endif  // COMMON_SECURITY_REGISTRATION_H_
