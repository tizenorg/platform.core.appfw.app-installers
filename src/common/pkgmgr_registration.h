// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_PKGMGR_REGISTRATION_H_
#define COMMON_PKGMGR_REGISTRATION_H_

#include <boost/filesystem.hpp>
#include <pkgmgr-info.h>
#include <unistd.h>

#include <string>

#include "common/context_installer.h"

namespace common_installer {

bool RegisterAppInPkgmgr(const boost::filesystem::path& xml_path,
                         const std::string& pkgid,
                         const CertificateInfo& cert_info, uid_t uid);
bool UpgradeAppInPkgmgr(const boost::filesystem::path& xml_path,
                        const std::string& pkgid,
                        const CertificateInfo& cert_info, uid_t uid);
bool UnregisterAppInPkgmgr(const boost::filesystem::path& xml_path,
                           const std::string& pkgid, uid_t uid);
std::string QueryCertificateAuthorCertificate(const std::string& pkgid,
                                              uid_t uid);
bool IsPackageInstalled(const std::string& pkg_id, uid_t uid);

}  // namespace common_installer

#endif  // COMMON_PKGMGR_REGISTRATION_H_
