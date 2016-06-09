// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/configuration/step_check_tizen_version.h"

#include <manifest_parser/utils/version_number.h>

#include <string>

#include "common/installer_context.h"

namespace {

const char kPlatformVersion[] = TIZEN_FULL_VERSION;

int CompareVersion(const std::string& a, const std::string& b) {
  utils::VersionNumber a_ver(a);
  utils::VersionNumber b_ver(b);

  return a_ver.Trim(3).Compare(b_ver.Trim(3));
}

}  // namespace

namespace common_installer {
namespace configuration {

Step::Status StepCheckTizenVersion::process() {
  const char* version = context_->manifest_data.get()->api_version;
  if (CompareVersion(std::string(kPlatformVersion), std::string(version)) < 0) {
    LOG(ERROR) << "Package's api version(" << version << ") is higher than "
               << "platform version(" << kPlatformVersion << ")";
    return Status::OPERATION_NOT_ALLOWED;
  }
  return Status::OK;
}

Step::Status StepCheckTizenVersion::precheck() {
  const char* version = context_->manifest_data.get()->api_version;
  if (version == nullptr) {
    LOG(ERROR) << "Api version not found";
    return Status::INVALID_VALUE;
  }
  return Status::OK;
}

}  // namespace configuration
}  // namespace common_installer
