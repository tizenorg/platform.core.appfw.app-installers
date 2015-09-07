// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_SECURITY_REGISTRATION_H_
#define COMMON_SECURITY_REGISTRATION_H_

#include <boost/filesystem/path.hpp>

#include <sys/types.h>

#include <string>

#include "common/context_installer.h"

namespace common_installer {

bool RegisterSecurityContext(const std::string& app_id,
    const std::string& pkg_id, const boost::filesystem::path& path,
    const std::vector<std::string>& privileges);
bool UnregisterSecurityContext(const std::string& app_id,
    const std::string& pkg_id);
bool RegisterSecurityContextForManifest(const std::string& pkg_id,
    const boost::filesystem::path& path, manifest_x* manifest);
bool UnregisterSecurityContextForManifest(const std::string& pkg_id,
    manifest_x* manifest);

}  // namespace common_installer

#endif  // COMMON_SECURITY_REGISTRATION_H_
