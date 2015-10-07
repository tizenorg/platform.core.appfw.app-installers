// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include "common/step/step_privilege_compatibility.h"

#include <pkgmgrinfo_basic.h>
#include <security-manager.h>

#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>

#include "common/utils/glist_range.h"

namespace {

const char kPlatformVersion[] = "3.0";
const char kPrivForPublic[] =
    "http://tizen.org/privilege/internal/default/public";
const char kPrivForPartner[] =
    "http://tizen.org/privilege/internal/default/partner";
const char kPrivForPlatform[] =
    "http://tizen.org/privilege/internal/default/platform";

bool TranslatePrivilegesForCompatibility(manifest_x* m) {
  if (!m->api_version) {
    LOG(WARNING) << "Skipping privileges mapping because api-version "
                 << "is not specified by package";
    return true;
  }
  if (strcmp(m->api_version, kPlatformVersion) == 0)
    return true;

  // No privileges to map
  if (!m->privileges) {
    return true;
  }

  // prepare input structure
  std::unique_ptr<const char*[]> input_privileges(
      new const char*[g_list_length(m->privileges)]);
  size_t input_size = 0;
  for (const char* priv : GListRange<char*>(m->privileges)) {
    input_privileges[input_size++] = priv;
  }

  // get mapping
  size_t output_size = 0;
  char** output_privileges = nullptr;
  if (security_manager_get_privileges_mapping(m->api_version, kPlatformVersion,
      input_privileges.get(), input_size, &output_privileges, &output_size)
      != SECURITY_MANAGER_SUCCESS) {
    LOG(ERROR) << "security_manager_get_privileges_mapping failed";
    return false;
  }

  // set pkgmgr new list
  g_list_free_full(m->privileges, free);
  m->privileges = nullptr;
  for (size_t i = 0; i < output_size; ++i) {
    m->privileges = g_list_append(m->privileges, strdup(output_privileges[i]));
  }

  security_manager_privilege_mapping_free(output_privileges, output_size);
  return true;
}

}  // namespace

namespace common_installer {
namespace security {

Step::Status StepPrivilegeCompatibility::precheck() {
  if (!context_->manifest_data.get()) {
    LOG(ERROR) << "Manifest data is not set";
    return Status::ERROR;
  }
  return Status::OK;
}

Step::Status StepPrivilegeCompatibility::process() {
  // Add default privileges for each certificates level.
  bool ret = true;
  switch (context_->privilege_level.get()) {
    case common_installer::PrivilegeLevel::PUBLIC:
      context_->manifest_data.get()->privileges =
          g_list_append(context_->manifest_data.get()->privileges,
                        strdup(kPrivForPublic));
      break;
    case common_installer::PrivilegeLevel::PARTNER:
      context_->manifest_data.get()->privileges =
          g_list_append(context_->manifest_data.get()->privileges,
                        strdup(kPrivForPartner));
      break;
    case common_installer::PrivilegeLevel::PLATFORM:
      context_->manifest_data.get()->privileges =
          g_list_append(context_->manifest_data.get()->privileges,
                        strdup(kPrivForPlatform));
      break;
    default:
      // No default privileges for untrusted application.
      break;
  }
  if (!ret) {
    LOG(ERROR) << "Error during adding default privileges for certificates.";
    return Status::ERROR;
  }

  return TranslatePrivilegesForCompatibility(context_->manifest_data.get()) ?
      Status::OK : Status::ERROR;
}

}  // namespace security
}  // namespace common_installer

