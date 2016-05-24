// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "common/step/configuration/step_check_tizen_version.h"

#include <cstdio>

namespace {

const char kPlatformVersion[] = TIZEN_FULL_VERSION;

class TizenVersion {
 public:
  TizenVersion(const char* version);
  bool operator<(const TizenVersion& other);

 private:
  int major_;
  int minor_;
  int patch_;
};

TizenVersion::TizenVersion(const char* version) {
  sscanf(version, "%d.%d.%d", &major_, &minor_, &patch_);
}

bool TizenVersion::operator<(const TizenVersion& other) {
  if (major_ < other.major_)
    return true;
  else if (minor_ < other.minor_)
    return true;
  else if (patch_ < other.patch_)
    return true;
  else
    return false;
}

}  // namespace

namespace common_installer {
namespace configuration {

Step::Status StepCheckTizenVersion::process() {
  const char* version = context_->manifest_data.get()->api_version;
  TizenVersion pkg_ver(version);
  TizenVersion platform_ver(kPlatformVersion);
  if (platform_ver < pkg_ver) {
    LOG(ERROR) << "Package version(" << version << ") is higher than "
               << "platform version(" << kPlatformVersion << ")";
    return Status::OPERATION_NOT_ALLOWED;
  }
  return Status::OK;
}

Step::Status StepCheckTizenVersion::precheck() {
  const char* version = context_->manifest_data.get()->api_version;
  if (version == nullptr) {
    LOG(ERROR) << "Package version not found";
    return Status::INVALID_VALUE;
  }
  return Status::OK;
}

}  // namespace configuration
}  // namespace common_installer
