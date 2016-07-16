// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_PKGDIR_TOOL_REQUEST_H_
#define COMMON_PKGDIR_TOOL_REQUEST_H_

#include <string>

namespace common_installer {

/**
 * \brief Request to copy per-user directories
 *
 * \param pkgid package id
 *
 * If IPC fails, fall back to direct function call (for offline)
 *
 * \return bool true if succeed, false otherwise
 */
bool RequestCopyUserDirectories(const std::string& pkgid);

/**
 * \brief Request to delete per-user directories
 *
 * \param pkgid package id
 *
 * If IPC fails, fall back to direct function call (for offline)
 *
 * \return bool true if succeed, false otherwise
 */
bool RequestDeleteUserDirectories(const std::string& pkgid);

/**
 * \brief Request to create external directories
 *
 * \param pkgid package id
 *
 * If IPC fails, fall back to direct function call (for offline)
 *
 * \return bool true if succeed, false otherwise
 */
bool RequestCreateExternalDirectories(const std::string& pkgid);

/**
 * \brief Request to delete external directories
 *
 * \param pkgid package id
 *
 * If IPC fails, fall back to direct function call (for offline)
 *
 * \return bool true if succeed, false otherwise
 */
bool RequestDeleteExternalDirectories(const std::string& pkgid);

/**
 * \brief Request to create legacy directories
 *
 * \param pkgid package id
 *
 * \return bool true if succeed, false otherwise
 */
bool RequestCreateLegacyDirectories(const std::string& pkgid);

/**
 * \brief Request to delete legacy directories
 *
 * \param pkgid package id
 *
 * \return bool true if succeed, false otherwise
 */
bool RequestDeleteLegacyDirectories(const std::string& pkgid);

}  // namespace common_installer

#endif  // COMMON_PKGDIR_TOOL_REQUEST_H_
