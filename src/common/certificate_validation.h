// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#ifndef COMMON_CERTIFICATE_VALIDATION_H_
#define COMMON_CERTIFICATE_VALIDATION_H_

#include <boost/filesystem/path.hpp>
#include <privilege_manager.h>
#include <vcore/SignatureData.h>
#include <vcore/SignatureFinder.h>

#include <string>

#include "common/installer_context.h"

namespace common_installer {

common_installer::PrivilegeLevel CertStoreIdToPrivilegeLevel(
    ValidationCore::CertStoreId::Type id);

privilege_manager_visibility_e PrivilegeLevelToVisibility(
    common_installer::PrivilegeLevel level);

void SetPrivilegeLevel(ValidationCore::SignatureData data,
    common_installer::PrivilegeLevel* level);

bool ValidateSignatureFile(
    const boost::filesystem::path& base_path,
    const ValidationCore::SignatureFileInfo& file_info,
    common_installer::PrivilegeLevel* level,
    common_installer::CertificateInfo* cert_info,
    bool check_reference, std::string* error_message);

bool ValidateSignatures(const boost::filesystem::path& base_path,
    PrivilegeLevel* level, common_installer::CertificateInfo* cert_info,
    bool check_reference, std::string* error_message);

bool ValidatePrivilegeLevel(common_installer::PrivilegeLevel level,
    bool is_webapp, const char* api_version, GList* privileges,
    std::string* error_message);

}  // namespace common_installer

#endif  // COMMON_CERTIFICATE_VALIDATION_H_
